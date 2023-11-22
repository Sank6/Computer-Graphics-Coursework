#pragma once
#include "Object.h"

class Camera : public GenericObject {
public:
	float focalLength;
	Camera(DrawingWindow &window);
	glm::vec3 getRayDirection(int, int, int, int);
	CanvasPoint getCanvasIntersectionPoint(glm::vec3);
};