#pragma once

#include <glm/glm.hpp>
#include <CanvasPoint.h>
#include <DrawingWindow.h>
#include <ModelTriangle.h>

class GenericObject {
public:
	std::string name;
	glm::mat4 transformation;
	glm::mat4 startPosition;
    int screenWidth, screenHeight;

	GenericObject();
    GenericObject(DrawingWindow &window);

	glm::vec3 getPosition();
	void translate(glm::vec3);
	void transform(glm::mat4, bool);
	void setStartPosition();
	void reset();
	void rotateAroundPoint(glm::vec3, float, int);
};