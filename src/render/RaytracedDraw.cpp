#include "Draw.h"

uint32_t colourToInt(Colour colour) {
    return (255 << 24) + (int(colour.red) << 16) + (int(colour.green) << 8) + int(colour.blue);
}

RayTriangleIntersection getClosestValidIntersection(glm::vec3 rayDirection, glm::vec3 rayOrigin, std::vector<ModelTriangle> *scene) {
    RayTriangleIntersection closestIntersection;
    closestIntersection.distanceFromCamera = std::numeric_limits<float>::max();
    closestIntersection.triangleIndex = -1;

    for (size_t i = 0; i < scene->size(); i++) {
        glm::vec3 e0 = scene->at(i).vertices[1] - scene->at(i).vertices[0];
        glm::vec3 e1 = scene->at(i).vertices[2] - scene->at(i).vertices[0];
        glm::vec3 SPVector = rayOrigin - scene->at(i).vertices[0];
        glm::mat3 DEMatrix(-rayDirection, e0, e1);
        glm::vec3 possibleSolution = glm::inverse(DEMatrix) * SPVector;

        float t = possibleSolution.x; // Distance from camera to intersection
        float u = possibleSolution.y; // Distance along e0
        float v = possibleSolution.z; // Distance along e1

        if (u != u || v != v || t != t) continue;

        // if (u >= 0 && v >= 0 && u + v <= 1) std::cout << "I: " << i << " U: " << u << " V: " << v << " T: " << t << std::endl;

        if (u >= 0 && v >= 0 && u + v <= 1 && t > 0 && t < closestIntersection.distanceFromCamera) {
            closestIntersection.distanceFromCamera = t;
            closestIntersection.intersectionPoint = rayOrigin + t * rayDirection;
            closestIntersection.intersectedTriangle = scene->at(i);
            closestIntersection.triangleIndex = (long) i;
        }
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
    window.clearPixels();
    for (size_t x = 0; x < window.width; x++) {
        for (size_t y = 0; y < window.height; y++) {
            glm::vec3 rayDirection = camera.getRayDirection(x, y, window.width, window.height);
            glm::vec3 rayOrigin = camera.getPosition();
            RayTriangleIntersection closestIntersection = getClosestValidIntersection(rayDirection, rayOrigin, &scene);
            if (closestIntersection.triangleIndex != -1) {
                // std::cout << std::hex << closestIntersection.intersectedTriangle.colour << std::endl;
                window.setPixelColour(x, y, colourToInt(closestIntersection.intersectedTriangle.colour), 1);
            }
        }
        std::cout << "Row " << x << " of " << window.width << " rendered" << std::endl;
        window.renderFrame();
    }
    std::cout << "Rays traced" << std::endl;
}