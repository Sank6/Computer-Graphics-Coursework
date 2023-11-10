#include "Draw.h"

uint32_t colourToInt(Colour colour) {
    return (255 << 24) + (int(colour.red) << 16) + (int(colour.green) << 8) + int(colour.blue);
}


RayTriangleIntersection getClosestValidIntersection(glm::vec3 rayDirection, glm::vec3 rayOrigin, std::vector<ModelTriangle> *scene) {
    RayTriangleIntersection closestIntersection;
    closestIntersection.distanceFromCamera = std::numeric_limits<float>::max();
    closestIntersection.triangleIndex = -1;

    size_t len = scene->size();
    for (size_t i = 0; i < len; i++) {
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
            closestIntersection.intersectionPoint = glm::vec3(u, v, 1 - u - v);
        }
    }

    if (closestIntersection.triangleIndex == -1) return closestIntersection;

    size_t i = closestIntersection.triangleIndex;
    ModelTriangle& triangle = (*scene)[i];
    float u = closestIntersection.intersectionPoint.x;
    float v = closestIntersection.intersectionPoint.y;
    if (triangle.textureMap.pixels.size() == 0) {
        closestIntersection.textureColour = colourToInt(triangle.colour);
    } else {
        float textureX = triangle.texturePoints[0].x + u * (triangle.texturePoints[1].x - triangle.texturePoints[0].x) + v * (triangle.texturePoints[2].x - triangle.texturePoints[0].x);
        float textureY = triangle.texturePoints[0].y + u * (triangle.texturePoints[1].y - triangle.texturePoints[0].y) + v * (triangle.texturePoints[2].y - triangle.texturePoints[0].y);
        int index = std::max(0, std::min(int(textureY * triangle.textureMap.height), int(triangle.textureMap.height - 1))) * triangle.textureMap.width + std::max(0, std::min(int(textureX * triangle.textureMap.width), int(triangle.textureMap.width - 1)));
        closestIntersection.textureColour = triangle.textureMap.pixels[index];
    }

    return closestIntersection;
}

void fillScreen(uint32_t colour, DrawingWindow &window) {
    for (size_t x = 0; x < window.width; x++) {
        for (size_t y = 0; y < window.height; y++) {
            window.setPixelColour(x, y, colour, 1);
        }
    }
}

void Draw::drawSceneRayTraced() {
    for (size_t x = 0; x < window.width; x++) {
        for (size_t y = 0; y < window.height; y++) {
            glm::vec3 rayDirection = camera.getRayDirection(x, y, window.width, window.height);
            glm::vec3 rayOrigin = camera.getPosition();
            RayTriangleIntersection closestIntersection = getClosestValidIntersection(rayDirection, rayOrigin, &scene);
            if (closestIntersection.triangleIndex != -1) {
                ModelTriangle *triangle = &scene[closestIntersection.triangleIndex];
                window.setPixelColour(x, y, closestIntersection.textureColour, 1);
            }
        }
    }
}