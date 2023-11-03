#include "Camera.h"

Camera::Camera(DrawingWindow &window) {
	position = glm::vec3(0.0f, 0.0f, 4.0f);
	transformation = glm::mat3(1.0f);
	focalLength = 2.0f;
	screenWidth = window.width;
	screenHeight = window.height;
}

CanvasPoint Camera::getCanvasIntersectionPoint(glm::vec3 vertexPosition) {
	glm::mat3 transformationInverse = glm::inverse(transformation);
	vertexPosition = transformationInverse * vertexPosition;
	int scalingFactor = screenHeight * focalLength;
	glm::vec3 cameraToVertex = vertexPosition - position;
	int u = round(scalingFactor * (cameraToVertex.x / (focalLength - cameraToVertex.z)) + (screenWidth / 2));
	int v = screenHeight - round(scalingFactor * (cameraToVertex.y / (focalLength - cameraToVertex.z)) + (screenHeight / 2));
	float depth = - 1 / (cameraToVertex.z);
	return CanvasPoint(u, v, depth);
}

void Camera::translate(glm::vec3 translation) {
	position = position + translation;
}

void Camera::transform(glm::mat3 newTransformation) {
	transformation = newTransformation * transformation;
}

void Camera::reset() {
	position = glm::vec3(0.0f, 0.0f, 4.0f);
	transformation = glm::mat3(1.0f);
}

void Camera::rotateX(float angle) {
	glm::mat3 rotationMatrix = glm::mat3(1.0f);
	rotationMatrix[1][1] = cos(angle);
	rotationMatrix[1][2] = -sin(angle);
	rotationMatrix[2][1] = sin(angle);
	rotationMatrix[2][2] = cos(angle);
	transform(rotationMatrix);
}

void Camera::rotateY(float angle) {
	glm::mat3 rotationMatrix = glm::mat3(1.0f);
	rotationMatrix[0][0] = cos(angle);
	rotationMatrix[0][2] = sin(angle);
	rotationMatrix[2][0] = -sin(angle);
	rotationMatrix[2][2] = cos(angle);
	transform(rotationMatrix);
}

void Camera::rotateZ(float angle) {
	glm::mat3 rotationMatrix = glm::mat3(1.0f);
	rotationMatrix[0][0] = cos(angle);
	rotationMatrix[0][1] = -sin(angle);
	rotationMatrix[1][0] = sin(angle);
	rotationMatrix[1][1] = cos(angle);
	transform(rotationMatrix);
}