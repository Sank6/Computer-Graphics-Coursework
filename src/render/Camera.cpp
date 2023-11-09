#include "Camera.h"


Camera::Camera(DrawingWindow &window) {
	transformation = glm::mat4(1.0f);
	focalLength = 2.0f;
	screenWidth = window.width;
	screenHeight = window.height;
	this->translate(glm::vec3(0.0f, 0.0f, 4.0f));
}

CanvasPoint Camera::getCanvasIntersectionPoint(glm::vec3 vertexPosition) {
	glm::vec4 vertexPosition4 = glm::vec4(vertexPosition, 1.0f);
	glm::vec4 vertexPosition4Transformed = glm::inverse(transformation) * vertexPosition4;

	glm::vec3 vertexPosition3Transformed = glm::vec3(vertexPosition4Transformed.x, vertexPosition4Transformed.y, vertexPosition4Transformed.z) / vertexPosition4Transformed.w;

	int scalingFactor = screenHeight * focalLength;

	int u = round((screenWidth / 2) + scalingFactor * (vertexPosition3Transformed.x / (focalLength - vertexPosition3Transformed.z)));
	int v = round((screenHeight / 2) - scalingFactor * (vertexPosition3Transformed.y / (focalLength - vertexPosition3Transformed.z)));

	float depth = 1 / (focalLength - vertexPosition3Transformed.z);

	return CanvasPoint(u, v, depth);
}

glm::vec3 Camera::getPosition() {
	return glm::vec3(transformation[3][0], transformation[3][1], transformation[3][2]);
}

void Camera::translate(glm::vec3 translation) {
	transformation[3][0] += translation.x;
	transformation[3][1] += translation.y;
	transformation[3][2] += translation.z;
}

void Camera::transform(glm::mat4 newTransformation) {
	transformation = newTransformation * transformation;
}

void Camera::reset() {
	transformation = glm::mat4(1.0f);
	this->translate(glm::vec3(0.0f, 0.0f, 4.0f));
}

void Camera::rotateAroundPoint(glm::vec3 point, float angle, Axis axis) {
	glm::mat4 translateToPoint = {
		1.0f, 0.0f, 0.0f, -point.x,
		0.0f, 1.0f, 0.0f, -point.y,
		0.0f, 0.0f, 1.0f, -point.z,
		0.0f, 0.0f, 0.0f, 1.0f
	};

	glm::mat4 rotationX = {
		1, 0, 0, 0,
		0, cos(angle), -sin(angle), 0,
		0, sin(angle), cos(angle), 0,
		0, 0, 0, 1
	};

	glm::mat4 rotationY = {
		cos(angle), 0, sin(angle), 0,
		0, 1, 0, 0,
		-sin(angle), 0, cos(angle), 0,
		0, 0, 0, 1
	};

	glm::mat4 rotationZ = {
		cos(angle), -sin(angle), 0, 0,
		sin(angle),  cos(angle), 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};

	glm::mat4 translateBack = {
		1, 0, 0, point.x,
		0, 1, 0, point.y,
		0, 0, 1, point.z,
		0, 0, 0, 1
	};

	if (axis == X) this->transform(translateToPoint * rotationX * translateBack);
	if (axis == Y) this->transform(translateToPoint * rotationY * translateBack);
	if (axis == Z) this->transform(translateToPoint * rotationZ * translateBack);
}

glm::vec3 Camera::getRayDirection(int u, int v, int screenWidth, int screenHeight) {
	float scalingFactor = screenHeight * focalLength;
	float x = (u - (screenWidth / 2)) / scalingFactor;
	float y = ((screenHeight / 2) - v) / scalingFactor;
	float z = -1.0f / focalLength;

	glm::vec4 rayDirection4 = glm::vec4(x, y, z, 0.0f);
	glm::vec4 rayDirection4Transformed = glm::inverse(transformation) * rayDirection4;

	glm::vec3 rayDirection3Transformed = glm::vec3(rayDirection4Transformed.x, rayDirection4Transformed.y, rayDirection4Transformed.z);

	return glm::normalize(rayDirection3Transformed);
}