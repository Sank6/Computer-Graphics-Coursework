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
    red = std::max(0, std::min(255, (int) red));
    green = std::max(0, std::min(255, (int) green));
    blue = std::max(0, std::min(255, (int) blue));
    return (255 << 24) + (red << 16) + (green << 8) + blue;
}

RayTriangleIntersection getClosestValidIntersection(glm::vec3 rayDirection, glm::vec3 rayOrigin, std::vector<ModelTriangle> *scene, std::vector<Light> *lights, int triangleIndex = -1, int lightIndex = -1) {
    RayTriangleIntersection closestIntersection = RayTriangleIntersection();
    closestIntersection.distanceFromCamera = FLT_MAX;
    closestIntersection.triangleIndex = -1;

    bool isShadowRay = lightIndex != -1;
    if (isShadowRay) closestIntersection.distanceFromCamera = glm::distance(rayOrigin, (*lights)[lightIndex].position);

    size_t len = scene->size();
    for (size_t i = 0; i < len; i++) {
        if (i == (size_t) triangleIndex) continue;
        ModelTriangle& triangle = (*scene)[i];
        glm::vec3 e0 = triangle.vertices[1] - triangle.vertices[0];
        glm::vec3 e1 = triangle.vertices[2] - triangle.vertices[0];
        glm::vec3 SPVector = rayOrigin - triangle.vertices[0];
        glm::mat3 DEMatrix(-rayDirection, e0, e1);
        glm::vec3 possibleSolution = glm::inverse(DEMatrix) * SPVector;

        float t = possibleSolution.x; // Distance from camera to intersection
        float u = possibleSolution.y; // Distance along e0
        float v = possibleSolution.z; // Distance along e1

        if (std::isnan(t) || std::isnan(u) || std::isnan(v)) continue;

        if (u >= 0 && v >= 0 && u + v <= 1 && t > 0 && t < closestIntersection.distanceFromCamera) {
            closestIntersection.distanceFromCamera = t;
            closestIntersection.triangleIndex = (long) i;
            closestIntersection.intersectionPoint = rayOrigin + rayDirection * t;
            closestIntersection.u = u;
            closestIntersection.v = v;
            if (isShadowRay) return closestIntersection;
        }
    }

    if (isShadowRay) closestIntersection.triangleIndex = -1;
    if (closestIntersection.triangleIndex == -1) return closestIntersection;

    size_t i = closestIntersection.triangleIndex;
    ModelTriangle& triangle = (*scene)[i];
    if (triangle.textureMap.pixels.size() == 0) {
        closestIntersection.textureColour = colourToInt(triangle.colour);
    } else {
        float textureX = triangle.texturePoints[0].x + closestIntersection.u * (triangle.texturePoints[1].x - triangle.texturePoints[0].x) + closestIntersection.v * (triangle.texturePoints[2].x - triangle.texturePoints[0].x);
        float textureY = triangle.texturePoints[0].y + closestIntersection.u * (triangle.texturePoints[1].y - triangle.texturePoints[0].y) + closestIntersection.v * (triangle.texturePoints[2].y - triangle.texturePoints[0].y);
        int index = std::max(0, std::min(int(textureY * triangle.textureMap.height), int(triangle.textureMap.height - 1))) * triangle.textureMap.width + std::max(0, std::min(int(textureX * triangle.textureMap.width), int(triangle.textureMap.width - 1)));
        closestIntersection.textureColour = triangle.textureMap.pixels[index];
    }

    float factor = 1.0f;

    // Ambient lighting
    float ambient = 3.5f;
    factor *= ambient;

    for (size_t j = 0; j < lights->size(); j++) {
        glm::vec3 lightRayDirection = glm::normalize((*lights)[j].position - closestIntersection.intersectionPoint);
        glm::vec3 lightRayOrigin = closestIntersection.intersectionPoint;
        
        float distanceToLight = glm::distance(closestIntersection.intersectionPoint, (*lights)[j].position);
        float angleOfIncidence = glm::dot(triangle.normal, -lightRayDirection);
        
        glm::vec3 viewDirection = glm::normalize(rayOrigin - closestIntersection.intersectionPoint);
        glm::vec3 reflectDirection = glm::reflect(-lightRayDirection, triangle.normal);

        // Light falloff
        float falloff = 2.2f;
        float falloffShadowFactor = 1 / std::max(1.0f, falloff * distanceToLight * distanceToLight);

        // Or shadows (reassign the colour variable if the point is a shadow)
        float shadow = 1.5f;
        RayTriangleIntersection shadowIntersection = getClosestValidIntersection(lightRayDirection, lightRayOrigin, scene, lights, i, j);
        if (shadowIntersection.triangleIndex != -1) falloffShadowFactor = 1 / std::max(1.0f, shadow * falloff * distanceToLight * distanceToLight);

        factor *= falloffShadowFactor;

        // Diffuse lighting
        float diffuse = std::max(0.0f, glm::dot(triangle.normal, -lightRayDirection));
        float diffuseClamp = 0.5f;
        factor *= diffuse * diffuseClamp + 1 - diffuseClamp;

        // Angle of incidence lighting
        float AoIclamp = 0.3f;
        if (angleOfIncidence > 0) factor *= angleOfIncidence * AoIclamp + 1 - AoIclamp;

        // Specular lighting
        float shininess = 512.0f;
        float specular = std::pow(std::max(0.0f, glm::dot(viewDirection, reflectDirection)), shininess);
        float specularClamp = 0.5f;
        factor *= specular * specularClamp + 1 - specularClamp;

        // Gouraud smoothing
        

        // Light intensity and add to pixel
        factor *= (*lights)[j].intensity;
    }

    // Add to pixel
    closestIntersection.textureColour = brighten(closestIntersection.textureColour, factor);

    return closestIntersection;
}

void Draw::drawSceneRayTraced() {
    for (size_t x = 0; x < window.width; x++) {
        for (size_t y = 0; y < window.height; y++) {
            glm::vec3 rayDirection = camera.getRayDirection(x, y, window.width, window.height);
            glm::vec3 rayOrigin = camera.getPosition();
            RayTriangleIntersection closestIntersection = getClosestValidIntersection(rayDirection, rayOrigin, &scene, &lights);
            if (closestIntersection.triangleIndex != -1) 
                window.setPixelColour(x, y, closestIntersection.textureColour, 1);
        }
    }
}