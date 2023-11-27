#include "Draw.h"

RayTriangleIntersection getClosestValidIntersection(glm::vec3& rayDirection, glm::vec3& rayOrigin, Scene* scene, float refractiveIndex, int bounces);

float addLight(Light& light, RayTriangleIntersection& intersection, Scene* scene, glm::vec3& rayOrigin, glm::vec3& normal) {
  float brightness = 0.1f;
  Object3d& object = scene->objects[intersection.objectIndex];

  glm::vec3 lightRayDirection = glm::normalize(light.position - intersection.intersectionPoint);
  glm::vec3 lightRayOrigin = intersection.intersectionPoint + lightRayDirection * 0.001f;

  float distanceToLight = glm::distance(intersection.intersectionPoint, light.position);
  float angleOfIncidence = glm::dot(normal, -lightRayDirection);

  glm::vec3 viewDirection = glm::normalize(rayOrigin - intersection.intersectionPoint);
  glm::vec3 reflectDirection = glm::normalize(glm::reflect(-lightRayDirection, normal));

  // Shadows
  if (object.shading == FLAT && scene->shadowPass) {
    float shadow = 0.1f;
    glm::vec3 offsetLightRayDirection = lightRayDirection;
    RayTriangleIntersection shadowIntersection = getClosestValidIntersection(offsetLightRayDirection, lightRayOrigin, scene, 1.0f, 0);
    if (shadowIntersection.triangleIndex != -1 && glm::distance(shadowIntersection.intersectionPoint, lightRayOrigin) < distanceToLight)
      brightness -= shadow;
  }

  /* Specular lighting */
  if (scene->specularPass) {
    float shininess = 128.0f;
    float specular = std::pow(std::max(0.0f, glm::dot(viewDirection, reflectDirection)), shininess);
    brightness += specular * light.intensity * object.specularStrength;
  }

  /* Diffuse lighting */

  // Light falloff
  if (scene->falloffPass) {
    float falloff = 2.6f;
    brightness += light.intensity / std::max(0.01f, falloff * distanceToLight * distanceToLight);
  }

  // Angle of incidence lighting
  if (scene->aoiPass) {
    float AoImultiplier = 0.1f;
    brightness -= angleOfIncidence * AoImultiplier;
  }

  return std::min(2.0f, std::max(0.0f, brightness));
}

RayTriangleIntersection getClosestValidIntersection(glm::vec3& rayDirection, glm::vec3& rayOrigin, Scene* scene, float refractiveIndex = 1.0f, int bounces = 0) {
  RayTriangleIntersection closestIntersection = RayTriangleIntersection();
  closestIntersection.distanceFromCamera = FLT_MAX;
  closestIntersection.triangleIndex = -1;

  for (size_t j = 0; j < scene->objects.size(); j++) {
    Object3d& object = scene->objects[j];
    if (!intersectsAABB(rayOrigin, rayDirection, object.boundingBox)) continue;
    for (size_t i = 0; i < object.triangles.size(); i++) {
      ModelTriangle& triangle = object.triangles[i];
      glm::vec3 e0 = triangle.vertices[1] - triangle.vertices[0];
      glm::vec3 e1 = triangle.vertices[2] - triangle.vertices[0];
      glm::vec3 SPVector = rayOrigin - triangle.vertices[0];
      glm::mat3 DEMatrix(-rayDirection, e0, e1);
      glm::vec3 possibleSolution = glm::inverse(DEMatrix) * SPVector;

      float t = possibleSolution.x;  // Distance from camera to intersection
      float u = possibleSolution.y;  // Distance along e0
      float v = possibleSolution.z;  // Distance along e1

      if (std::isnan(t) || std::isnan(u) || std::isnan(v)) continue;

      if (u >= 0 && v >= 0 && u + v <= 1 && t > 0 && t < closestIntersection.distanceFromCamera) {
        closestIntersection.distanceFromCamera = t;
        closestIntersection.triangleIndex = (long)i;
        closestIntersection.objectIndex = (long)j;
        closestIntersection.intersectionPoint = rayOrigin + rayDirection * t;
        closestIntersection.u = u;
        closestIntersection.v = v;
      }
    }
  }
  if (closestIntersection.triangleIndex == -1 || bounces == 0) return closestIntersection;

  size_t i = closestIntersection.objectIndex;
  Object3d& object = scene->objects[i];

  size_t j = closestIntersection.triangleIndex;
  ModelTriangle& triangle = object.triangles[j];

  closestIntersection.refractiveIndex = object.refractiveIndex / refractiveIndex;

  if (triangle.textureMap.pixels.size() == 0) {
    closestIntersection.textureColour = colourToInt(triangle.colour);
  } else {
    float textureX = triangle.texturePoints[0].x + closestIntersection.u * (triangle.texturePoints[1].x - triangle.texturePoints[0].x) + closestIntersection.v * (triangle.texturePoints[2].x - triangle.texturePoints[0].x);
    float textureY = triangle.texturePoints[0].y + closestIntersection.u * (triangle.texturePoints[1].y - triangle.texturePoints[0].y) + closestIntersection.v * (triangle.texturePoints[2].y - triangle.texturePoints[0].y);
    int index = std::max(0, std::min(int(textureY * triangle.textureMap.height), int(triangle.textureMap.height - 1))) * triangle.textureMap.width + std::max(0, std::min(int(textureX * triangle.textureMap.width), int(triangle.textureMap.width - 1)));
    closestIntersection.textureColour = triangle.textureMap.pixels[index];
  }

  glm::vec3 normal = triangle.normal;
  if (object.shading == PHONG) normal = glm::normalize(triangle.vertexNormals[0] + closestIntersection.u * (triangle.vertexNormals[1] - triangle.vertexNormals[0]) + closestIntersection.v * (triangle.vertexNormals[2] - triangle.vertexNormals[0]));

  float brightness = 0.0f;
  for (size_t j = 0; j < scene->lights.size(); j++) {
    Light& light = scene->lights[j];
    brightness += addLight(light, closestIntersection, scene, rayOrigin, normal);
  }

  // Ambient lighting
  if (scene->ambientPass) {
    float ambient = 0.1f;
    brightness += ambient;
  }

  // Add to pixel
  closestIntersection.textureColour = brighten(closestIntersection.textureColour, brightness);

  /* Reflections */
  if (scene->reflectionPass && bounces > 1) {
    glm::vec3 incidentDirection = glm::normalize(closestIntersection.intersectionPoint - rayOrigin);
    glm::vec3 reflectDirection = glm::normalize(glm::reflect(incidentDirection, normal));
    glm::vec3 offsetOrigin = closestIntersection.intersectionPoint + reflectDirection * 0.001f;
    RayTriangleIntersection reflectIntersection = getClosestValidIntersection(reflectDirection, offsetOrigin, scene, refractiveIndex, bounces - 1);
    if (reflectIntersection.triangleIndex != -1)
      closestIntersection.textureColour = combine(closestIntersection.textureColour, reflectIntersection.textureColour, 1 - object.reflectiveness);
    else {
      uint32_t backgroundColour = colourToInt(scene->backgroundColour);
      closestIntersection.textureColour = combine(closestIntersection.textureColour, backgroundColour, 1 - object.reflectiveness);
    }
  }

  /* Refractions */
  if (scene->refractionPass && bounces > 1) {
    glm::vec3 incidentDirection = glm::normalize(closestIntersection.intersectionPoint - rayOrigin);
    float n1 = refractiveIndex;
    float n2 = closestIntersection.refractiveIndex;
    float n = n1 / n2;
    if (glm::dot(incidentDirection, normal) > 0) normal = -normal;
    float cosI = -glm::dot(incidentDirection, normal);
    float sinT2 = n * n * (1.0f - cosI * cosI);
    if (sinT2 > 1.0f) return closestIntersection;
    float cosT = std::sqrt(1.0f - sinT2);
    glm::vec3 refractDirection = glm::normalize(n * incidentDirection + (n * cosI - cosT) * normal);
    glm::vec3 offsetOrigin = closestIntersection.intersectionPoint + refractDirection * 0.001f;
    RayTriangleIntersection refractIntersection = getClosestValidIntersection(refractDirection, offsetOrigin, scene, closestIntersection.refractiveIndex, bounces - 1);
    if (refractIntersection.triangleIndex != -1)
      closestIntersection.textureColour = combine(closestIntersection.textureColour, refractIntersection.textureColour, 1 - object.transparency);
  }

  return closestIntersection;
}

void renderRows(int startRow, int endRow, DrawingWindow& window, Scene& scene) {
  uint32_t backgroundColour = colourToInt(scene.backgroundColour);
  for (int x = startRow; x < endRow; x++) {
    for (size_t y = 0; y < window.height; y++) {
      glm::vec3 rayDirection = scene.camera.getRayDirection(x, y, window.width, window.height);
      glm::vec3 rayOrigin = scene.camera.getPosition();
      RayTriangleIntersection closestIntersection = getClosestValidIntersection(rayDirection, rayOrigin, &scene, 1.0f, 4);
      if (closestIntersection.triangleIndex != -1)
        window.setPixelColour(x, y, closestIntersection.textureColour, 1);
      else
        window.setPixelColour(x, y, backgroundColour, 1);
    }
  }
}

void Draw::drawSceneRayTraced() {
  int numThreads = 16;
  std::vector<std::thread> threads(numThreads);

  int rowsPerThread = window.width / numThreads;
  for (int i = 0; i < numThreads; ++i) {
    int startRow = i * rowsPerThread;
    int endRow = (i + 1) * rowsPerThread;
    if (i == numThreads - 1) endRow = window.width;
    threads[i] = std::thread(renderRows, startRow, endRow, std::ref(window), std::ref(scene));
  }

  for (auto& t : threads) t.join();
}