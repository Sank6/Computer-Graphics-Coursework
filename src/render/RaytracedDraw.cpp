#include "Draw.h"

RayTriangleIntersection getClosestValidIntersection(glm::vec3& rayDirection, glm::vec3& rayOrigin, Scene* scene, float refractiveIndex, int bounces);

template <size_t N>
void gen_offsets(int num, float mul, std::array<glm::vec3, N>& offsets) {
  thread_local std::random_device rd;
  thread_local std::mt19937 gen(rd());
  thread_local std::uniform_real_distribution<float> dis(-0.5f, 0.5f);

  for (int i = 0; i < num; i++) {
    float angle = 2 * M_PI * i / num;
    offsets[i] = glm::vec3(std::cos(angle) * mul, std::sin(angle) * mul, 0.0f);
    if (dis(gen) < 0.0f && num > 1) offsets[i] += glm::vec3(dis(gen) * mul, dis(gen) * mul, 0.0f);
  }
}

float addLight(Light& light, RayTriangleIntersection& intersection, Scene* scene, glm::vec3& rayOrigin, glm::vec3& normal) {
  float brightness = 0.1f;
  Object3d& object = scene->objects[intersection.objectIndex];

  glm::vec3 lightRayDirection = glm::normalize(light.getPosition() - intersection.intersectionPoint);

  float distanceToLight = glm::distance(intersection.intersectionPoint, light.getPosition());
  float angleOfIncidence = glm::dot(normal, -lightRayDirection);

  glm::vec3 viewDirection = glm::normalize(rayOrigin - intersection.intersectionPoint);
  glm::vec3 reflectDirection = glm::normalize(glm::reflect(-lightRayDirection, normal));

  // Shadows
  if (object.shading == FLAT && scene->shadowPass) {
    float shadowMul = 0.2f;
    float shadow = 0.0f;
    const int numShadowRays = 1;

    thread_local std::array<glm::vec3, numShadowRays> offsets;
    gen_offsets(numShadowRays, 0.025f, offsets);

    for (int i = 0; i < numShadowRays; i++) {
      glm::vec3 offsetBy = offsets[i];
      glm::vec3 shadowRayDirection = glm::normalize(light.getPosition() - intersection.intersectionPoint + offsetBy);
      glm::vec3 shadowRayOrigin = intersection.intersectionPoint + shadowRayDirection * 0.001f;
      RayTriangleIntersection shadowIntersection = getClosestValidIntersection(shadowRayDirection, shadowRayOrigin, scene, 1.0f, 0);
      if (shadowIntersection.triangleIndex != -1 && glm::distance(shadowIntersection.intersectionPoint, shadowRayOrigin) < distanceToLight)
        shadow += shadowMul;
    }
    brightness -= shadow / numShadowRays;
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
      glm::vec3 e0 = triangle.transformedVertices[1] - triangle.transformedVertices[0];
      glm::vec3 e1 = triangle.transformedVertices[2] - triangle.transformedVertices[0];
      glm::vec3 SPVector = rayOrigin - triangle.transformedVertices[0];
      glm::mat3 DEMatrix(-rayDirection, e0, e1);
      glm::vec3 possibleSolution = glm::inverse(DEMatrix) * SPVector;

      float t = possibleSolution.x;  // Distance from camera to intersection
      float u = possibleSolution.y;  // Distance along e0
      float v = possibleSolution.z;  // Distance along e1

      glm::vec3 normal = glm::normalize(glm::cross(e0, e1));
      if (glm::dot(rayDirection, normal) > 0 && object.cull) continue;

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

  if (object.shading == GOURAUD) {
    brightness = triangle.vertexBrightness[0] + closestIntersection.u * (triangle.vertexBrightness[1] - triangle.vertexBrightness[0]) + closestIntersection.v * (triangle.vertexBrightness[2] - triangle.vertexBrightness[0]);
  } else {
    for (size_t j = 0; j < scene->lights.size(); j++) {
      Light& light = scene->lights[j];
      brightness += addLight(light, closestIntersection, scene, rayOrigin, normal);
    }
  }

  // Ambient lighting
  if (scene->ambientPass) {
    float ambient = 0.1f;
    brightness += ambient;
  }

  // Add to pixel
  closestIntersection.textureColour = brighten(closestIntersection.textureColour, brightness);

  /* Reflections */
  if (scene->reflectionPass && bounces > 1 && object.reflectiveness > 0.0f) {
    glm::vec3 incidentDirection = glm::normalize(closestIntersection.intersectionPoint - rayOrigin);
    glm::vec3 reflectDirection = glm::normalize(glm::reflect(incidentDirection, normal));
    glm::vec3 offsetOrigin = closestIntersection.intersectionPoint + reflectDirection * 0.001f;
    RayTriangleIntersection reflectIntersection = getClosestValidIntersection(reflectDirection, offsetOrigin, scene, refractiveIndex, bounces - 1);
    if (reflectIntersection.triangleIndex != -1)
      closestIntersection.textureColour = combine(closestIntersection.textureColour, reflectIntersection.textureColour, 1 - object.reflectiveness);
    else {
      if (scene->environmentMap.pixels.size() > 0) {
        float textureX = 0.5f + std::atan2(reflectDirection.z, reflectDirection.x) / (2 * M_PI);
        float textureY = 0.5f - std::asin(reflectDirection.y) / M_PI;
        int index = std::max(0, std::min(int(textureY * scene->environmentMap.height), int(scene->environmentMap.height - 1))) * scene->environmentMap.width + std::max(0, std::min(int(textureX * scene->environmentMap.width), int(scene->environmentMap.width - 1)));
        uint32_t envMapColour = scene->environmentMap.pixels[index];

        closestIntersection.textureColour = combine(closestIntersection.textureColour, envMapColour, 1 - object.reflectiveness);
      } else
        closestIntersection.textureColour = combine(closestIntersection.textureColour, colourToInt(scene->backgroundColour), 1 - object.reflectiveness);
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
    else {
      if (scene->environmentMap.pixels.size() > 0) {
        float textureX = 0.5f + std::atan2(refractDirection.z, refractDirection.x) / (2 * M_PI);
        float textureY = 0.5f - std::asin(refractDirection.y) / M_PI;
        int index = std::max(0, std::min(int(textureY * scene->environmentMap.height), int(scene->environmentMap.height - 1))) * scene->environmentMap.width + std::max(0, std::min(int(textureX * scene->environmentMap.width), int(scene->environmentMap.width - 1)));
        uint32_t envMapColour = scene->environmentMap.pixels[index];

        closestIntersection.textureColour = combine(closestIntersection.textureColour, envMapColour, 1 - object.transparency);
      } else
        closestIntersection.textureColour = combine(closestIntersection.textureColour, colourToInt(scene->backgroundColour), 1 - object.transparency);
    }
  }

  return closestIntersection;
}

void renderRows(int startRow, int endRow, DrawingWindow& window, Scene& scene) {
  uint32_t backgroundColour = colourToInt(scene.backgroundColour);
  for (int x = startRow; x < endRow; x++) {
    for (size_t y = 0; y < window.height; y++) {
      glm::vec3 rayDirection = scene.camera.getRayDirection(x, y, window.width, window.height);
      glm::vec3 rayOrigin = scene.camera.getPosition();
      RayTriangleIntersection closestIntersection = getClosestValidIntersection(rayDirection, rayOrigin, &scene, 1.0f, 10);
      if (closestIntersection.triangleIndex != -1)
        window.setPixelColour(x, y, closestIntersection.textureColour, 1);
      else {
        if (scene.environmentMap.pixels.size() > 0) {
          float textureX = 0.5f + std::atan2(rayDirection.z, rayDirection.x) / (2 * M_PI);
          float textureY = 0.5f - std::asin(rayDirection.y) / M_PI;
          int index = std::max(0, std::min(int(textureY * scene.environmentMap.height), int(scene.environmentMap.height - 1))) * scene.environmentMap.width + std::max(0, std::min(int(textureX * scene.environmentMap.width), int(scene.environmentMap.width - 1)));
          window.setPixelColour(x, y, scene.environmentMap.pixels[index], 1);
        } else
          window.setPixelColour(x, y, backgroundColour, 1);
      }
    }
  }
}

void Draw::drawSceneRayTraced() {
  // GOURAUD shading
  for (size_t i = 0; i < scene.objects.size(); i++) {
    Object3d& object = scene.objects[i];
    for (size_t j = 0; j < object.triangles.size(); j++) {
      ModelTriangle& triangle = object.triangles[j];
      if (object.shading == GOURAUD) {
        RayTriangleIntersection constructedIntersection = RayTriangleIntersection();
        constructedIntersection.objectIndex = i;
        constructedIntersection.triangleIndex = j;
        for (size_t k = 0; k < 3; k++) {
          constructedIntersection.intersectionPoint = triangle.transformedVertices[k];
          glm::vec3 normal = triangle.vertexNormals[k];
          
          triangle.vertexBrightness[k] = 0.0f;
          for (Light& light : scene.lights) {
            glm::vec3 position = scene.camera.getPosition();
            float brightness = addLight(light, constructedIntersection, &scene, position, normal);
            triangle.vertexBrightness[k] += brightness;
          }
        }
      }
    }
  }

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