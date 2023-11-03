#include <glm/glm.hpp>
#include <CanvasPoint.h>
#include <DrawingWindow.h>

class Camera {
private:
	glm::mat3 transformation;
	float focalLength;
	int screenWidth, screenHeight;
public:
	glm::vec3 position;
	Camera(DrawingWindow &window);
	CanvasPoint getCanvasIntersectionPoint(glm::vec3);
	void translate(glm::vec3);
	void transform(glm::mat3);
	void reset();
	void rotateX(float);
	void rotateY(float);
	void rotateZ(float);
};