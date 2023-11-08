#include <glm/glm.hpp>
#include <CanvasPoint.h>
#include <DrawingWindow.h>

class Camera {
private:
	glm::mat3 transformation;
	int screenWidth, screenHeight;
public:
	glm::vec3 position;
	float focalLength;
	Camera(DrawingWindow &window);
	CanvasPoint getCanvasIntersectionPoint(glm::vec3);
	glm::vec3 getRayDirection(int, int, int, int);
	void translate(glm::vec3);
	void transform(glm::mat3);
	void reset();
	void rotateX(float);
	void rotateY(float);
	void rotateZ(float);
};