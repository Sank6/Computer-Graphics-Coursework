#include <glm/glm.hpp>
#include <CanvasPoint.h>
#include <DrawingWindow.h>

enum Axis { X, Y, Z };

class Camera {
private:
	int screenWidth, screenHeight;
public:
	glm::mat4 transformation;
	float focalLength;
	Camera(DrawingWindow &window);
	CanvasPoint getCanvasIntersectionPoint(glm::vec3);
	glm::vec3 getRayDirection(int, int, int, int);
	glm::vec3 getPosition();
	void translate(glm::vec3);
	void transform(glm::mat4);
	void reset();
	void rotateAroundPoint(glm::vec3, float, Axis);
};