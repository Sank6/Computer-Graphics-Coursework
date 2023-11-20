#ifndef CAMERA_H
#define CAMERA_H

#include "Object.h"

class Camera : public GenericObject {
public:
	float focalLength;
	Camera(DrawingWindow &window);
	glm::vec3 getRayDirection(int, int, int, int);
	CanvasPoint getCanvasIntersectionPoint(glm::vec3);
};

#endif // CAMERA_H