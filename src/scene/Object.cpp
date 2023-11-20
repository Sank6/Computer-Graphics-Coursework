#include "Object.h"

GenericObject::GenericObject(DrawingWindow &window) {
    transformation = glm::mat4(1.0f);
    this->screenWidth = window.width;
    this->screenHeight = window.height;
}

glm::vec3 GenericObject::getPosition() {
	return glm::vec3(transformation[3][0], transformation[3][1], transformation[3][2]);
}

void GenericObject::translate(glm::vec3 translation) {
	glm::mat4 translationMatrix = {
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0 ,1, 0,
		translation.x, translation.y, translation.z, 1
	};

	this->transform(translationMatrix, false);
}

void GenericObject::transform(glm::mat4 newTransformation, bool global = true) {
	if (global) transformation = newTransformation * transformation;
	else transformation = transformation * newTransformation;
}

void GenericObject::reset() {
	transformation = glm::mat4(1.0f);
	this->translate(glm::vec3(0.0f, 0.0f, 4.0f));
}

void GenericObject::rotateAroundPoint(glm::vec3 point, float angle, int axis) {
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

	this->translate(-point);

	if (axis == 0) this->transform(rotationX, true);
	if (axis == 1) this->transform(rotationY , true);
	if (axis == 2) this->transform(rotationZ, true);

	this->translate(point);
}