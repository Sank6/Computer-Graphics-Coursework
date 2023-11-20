#include "Draw.h"

uint32_t colourToInt(Colour colour) {
  return (255 << 24) + (int(colour.red) << 16) + (int(colour.green) << 8) + int(colour.blue);
}

uint32_t brighten(uint32_t colour, float factor) {
  uint32_t red = (colour >> 16) & 255;
  uint32_t green = (colour >> 8) & 255;
  uint32_t blue = colour & 255;
  red *= factor;
  green *= factor;
  blue *= factor;
  red = std::max(0, std::min(255, (int)red));
  green = std::max(0, std::min(255, (int)green));
  blue = std::max(0, std::min(255, (int)blue));
  return (255 << 24) + (red << 16) + (green << 8) + blue;
}

RayTriangleIntersection getClosestValidIntersection(glm::vec3 rayDirection, glm::vec3 rayOrigin, Scene* scene, int bounces = 0) {
  RayTriangleIntersection closestIntersection = RayTriangleIntersection();
  closestIntersection.distanceFromCamera = FLT_MAX;
  closestIntersection.triangleIndex = -1;

  for (size_t j = 0; j < scene->objects.size(); j++) {
    Object3d& object = scene->objects[j];
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

  size_t i = closestIntersection.triangleIndex;
  ModelTriangle& triangle = scene->objects[closestIntersection.objectIndex].triangles[i];
  if (triangle.textureMap.pixels.size() == 0) {
    closestIntersection.textureColour = colourToInt(triangle.colour);
  } else {
    float textureX = triangle.texturePoints[0].x + closestIntersection.u * (triangle.texturePoints[1].x - triangle.texturePoints[0].x) + closestIntersection.v * (triangle.texturePoints[2].x - triangle.texturePoints[0].x);
    float textureY = triangle.texturePoints[0].y + closestIntersection.u * (triangle.texturePoints[1].y - triangle.texturePoints[0].y) + closestIntersection.v * (triangle.texturePoints[2].y - triangle.texturePoints[0].y);
    int index = std::max(0, std::min(int(textureY * triangle.textureMap.height), int(triangle.textureMap.height - 1))) * triangle.textureMap.width + std::max(0, std::min(int(textureX * triangle.textureMap.width), int(triangle.textureMap.width - 1)));
    closestIntersection.textureColour = triangle.textureMap.pixels[index];
  }

  glm::vec3 interpolatedNormal = glm::normalize(triangle.vertexNormals[0] + closestIntersection.u * (triangle.vertexNormals[1] - triangle.vertexNormals[0]) + closestIntersection.v * (triangle.vertexNormals[2] - triangle.vertexNormals[0]));

  float brightness = 0.0f;
  for (size_t j = 0; j < scene->lights.size(); j++) {
    float factor = 1.0f;

    Light &light = scene->lights[j];

    glm::vec3 lightRayDirection = glm::normalize(light.position - closestIntersection.intersectionPoint);
    glm::vec3 lightRayOrigin = closestIntersection.intersectionPoint + lightRayDirection * 0.001f;

    float distanceToLight = glm::distance(closestIntersection.intersectionPoint, light.position);
    float angleOfIncidence = glm::dot(interpolatedNormal, -lightRayDirection);

    glm::vec3 viewDirection = glm::normalize(rayOrigin - closestIntersection.intersectionPoint);
    glm::vec3 reflectDirection = glm::reflect(-lightRayDirection, interpolatedNormal);

    // Light falloff
    float falloff = 1.6f;
    factor *= 1 / std::max(0.5f, falloff * distanceToLight * distanceToLight);

    // Or shadows (reassign the colour variable if the point is a shadow)
    float shadow = 0.5f;
    glm::vec3 offsetLightRayDirection = lightRayDirection;
    RayTriangleIntersection shadowIntersection = getClosestValidIntersection(offsetLightRayDirection, lightRayOrigin, scene, 0);
    if (shadowIntersection.triangleIndex != -1 && glm::distance(shadowIntersection.intersectionPoint, lightRayOrigin) < distanceToLight) {
      factor *= shadow;
    }

    // Diffuse lighting
    float diffuse = std::max(0.0f, glm::dot(interpolatedNormal, -lightRayDirection));
    float diffuseClamp = 0.5f;
    factor *= diffuse * diffuseClamp + 1 - diffuseClamp;

    // Angle of incidence lighting
    float AoIclamp = 0.9f;
    if (angleOfIncidence > 0) factor *= (1 - angleOfIncidence) * AoIclamp + 1 - AoIclamp;

    // Specular lighting
    float shininess = 1024.0f;
    float specular = std::pow(std::max(0.0f, glm::dot(viewDirection, reflectDirection)), shininess);
    float specularClamp = 0.3f;
    factor *= specular * specularClamp + 1 - specularClamp;

    // Light intensity and add to pixel
    factor *= light.intensity;

    brightness += factor;
  }

  // Ambient lighting
  float ambient = 0.1f;
  brightness += ambient;

  // Add to pixel
  closestIntersection.textureColour = brighten(closestIntersection.textureColour, brightness);

  return closestIntersection;
}

void renderRows(int startRow, int endRow, DrawingWindow& window, Scene& scene) {
  for (int x = startRow; x < endRow; x++) {
    for (size_t y = 0; y < window.height; y++) {
      glm::vec3 rayDirection = scene.camera.getRayDirection(x, y, window.width, window.height);
      glm::vec3 rayOrigin = scene.camera.getPosition();
      RayTriangleIntersection closestIntersection = getClosestValidIntersection(rayDirection, rayOrigin, &scene, 1);
      if (closestIntersection.triangleIndex != -1)
        window.setPixelColour(x, y, closestIntersection.textureColour, 1);
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