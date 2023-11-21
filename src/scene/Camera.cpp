#include "Camera.h"

Camera::Camera(DrawingWindow &window): GenericObject(window) {
	focalLength = 2.0f;	
}

glm::vec3 Camera::getRayDirection(int u, int v, int screenWidth, int screenHeight) {
	float scalingFactor = screenHeight * focalLength;
	float z = -0.6f;
	float x = (u - (screenWidth / 2)) / scalingFactor;
	float y = (v - (screenHeight / 2)) / -scalingFactor;

	glm::vec4 rayDirection4 = glm::vec4(x, y, z, 0.0f);
	glm::vec4 rayDirection4Transformed = transformation * rayDirection4;

	glm::vec3 rayDirection3Transformed = glm::vec3(rayDirection4Transformed.x, rayDirection4Transformed.y, rayDirection4Transformed.z);

	return glm::normalize(rayDirection3Transformed);
}

CanvasPoint Camera::getCanvasIntersectionPoint(glm::vec3 vertexPosition) {
	glm::vec4 vertexPos4 = glm::vec4(vertexPosition, 1.0f);
	glm::vec4 vertexPos4T = glm::inverse(transformation) * vertexPos4;

	glm::vec3 vertexPos3 = glm::vec3(vertexPos4T) / vertexPos4T.w;

	int scalingFactor = screenHeight * focalLength;

	int u = round((screenWidth / 2) + scalingFactor * (vertexPos3.x / (focalLength - vertexPos3.z)));
	int v = round((screenHeight / 2) - scalingFactor * (vertexPos3.y / (focalLength - vertexPos3.z)));

	float depth = 1 / (focalLength - vertexPos3.z);

	return CanvasPoint(u, v, depth);
}