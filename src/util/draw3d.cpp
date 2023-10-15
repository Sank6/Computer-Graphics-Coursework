#include "draw3d.h"
#include "lines.h"

CanvasPoint getCanvasIntersectionPoint(glm::vec3 cameraPosition, glm::vec3 vertexPosition, float focalLength, int screenWidth, int screenHeight) {
	int scalingFactor = screenHeight * focalLength;
	glm::vec3 cameraToVertex = vertexPosition - cameraPosition;
	int u = round(scalingFactor * (cameraToVertex.x / (focalLength - cameraToVertex.z)) + (screenWidth / 2));
	int v = screenHeight - round(scalingFactor * (cameraToVertex.y / (focalLength - cameraToVertex.z)) + (screenHeight / 2));
	return CanvasPoint(u, v);
}


void drawTriangle3D(DrawingWindow& window, ModelTriangle triangle) {
	Colour colour = triangle.colour;
	float focalLength = 2.0f;
	int screenWidth = window.width;
	int screenHeight = window.height;
	glm::vec3 cameraPosition = glm::vec3(0.0f, 0.0f, 4.0f);

	glm::vec3 vertex0 = triangle.vertices[0];
	glm::vec3 vertex1 = triangle.vertices[1];
	glm::vec3 vertex2 = triangle.vertices[2];

	CanvasPoint canvasPoint0 = getCanvasIntersectionPoint(cameraPosition, vertex0, focalLength, screenWidth, screenHeight);
	CanvasPoint canvasPoint1 = getCanvasIntersectionPoint(cameraPosition, vertex1, focalLength, screenWidth, screenHeight);
	CanvasPoint canvasPoint2 = getCanvasIntersectionPoint(cameraPosition, vertex2, focalLength, screenWidth, screenHeight);

	CanvasTriangle canvasTriangle = CanvasTriangle(canvasPoint0, canvasPoint1, canvasPoint2);

	drawFilledTriangle(window, canvasTriangle, colour);
	drawStrokedTriangle(window, canvasTriangle, colour);
}