#include "draw3d.h"
#include "lines.h"

CanvasPoint getCanvasIntersectionPoint(glm::vec3 cameraPosition, glm::vec3 vertexPosition, float focalLength, int screenWidth, int screenHeight) {
	int scalingFactor = screenHeight / 2;
	int u = round((scalingFactor * (vertexPosition.x - cameraPosition.x)) / (focalLength - (vertexPosition.z - cameraPosition.z)) + (screenWidth / 2));
	int v = round((scalingFactor * (vertexPosition.y - cameraPosition.y)) / (focalLength - (vertexPosition.z - cameraPosition.z)) + (screenHeight / 2));
	v = screenHeight - v;
	return CanvasPoint(u, v);
}

void drawLine3D(DrawingWindow& window, glm::vec3 vertex1, glm::vec3 vertex2, Colour colour) {
	float focalLength = 2.0f;
	int screenWidth = window.width;
	int screenHeight = window.height;
	glm::vec3 cameraPosition = glm::vec3(0.0f, 0.0f, 4.0f);

	CanvasPoint canvasPoint1 = getCanvasIntersectionPoint(cameraPosition, vertex1, focalLength, screenWidth, screenHeight);
	CanvasPoint canvasPoint2 = getCanvasIntersectionPoint(cameraPosition, vertex2, focalLength, screenWidth, screenHeight);

	drawLine(window, canvasPoint1, canvasPoint2, colour);
}

void drawTriangle3D(DrawingWindow& window, ModelTriangle triangle) {
	Colour colour = triangle.colour;
	drawLine3D(window, triangle.vertices[0], triangle.vertices[1], colour);
	drawLine3D(window, triangle.vertices[1], triangle.vertices[2], colour);
	drawLine3D(window, triangle.vertices[2], triangle.vertices[0], colour);
}