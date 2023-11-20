#ifndef GENERIC_OBJECT_H
#define GENERIC_OBJECT_H

#include <glm/glm.hpp>
#include <CanvasPoint.h>
#include <DrawingWindow.h>
#include <ModelTriangle.h>

class GenericObject {
public:
	glm::mat4 transformation;
    int screenWidth, screenHeight;

    GenericObject(DrawingWindow &window);

	glm::vec3 getPosition();
	void translate(glm::vec3);
	void transform(glm::mat4, bool);
	void reset();
	void rotateAroundPoint(glm::vec3, float, int);
};

#endif // GENERIC_OBJECT_H