#include "Draw.h"

std::vector<CanvasPoint> pointsOnLine(CanvasPoint from, CanvasPoint to) {
	std::vector<CanvasPoint> points = {};
	int numberOfSteps = std::max(abs(to.x - from.x), abs(to.y - from.y)) + 1;
	std::vector<float> linePointsX = interpolateSingleFloats(from.x, to.x, numberOfSteps);
	std::vector<float> linePointsY = interpolateSingleFloats(from.y, to.y, numberOfSteps);
	std::vector<float> linePointsZ = interpolateSingleFloats(from.depth, to.depth, numberOfSteps);

	for (int i = 0; i < numberOfSteps; i++) {
		points.push_back(CanvasPoint(floor(linePointsX[i]), floor(linePointsY[i]), linePointsZ[i]));
	}

	return points;
}

void drawLine(DrawingWindow& window, CanvasPoint from, CanvasPoint to, Colour colour) {
	u_int32_t colourInt = (255 << 24) + (int(colour.red) << 16) + (int(colour.green) << 8) + int(colour.blue);
	std::vector<CanvasPoint> linePoints = pointsOnLine(from, to);

	for (CanvasPoint point : linePoints) {
		if (point.x < 0 || point.x >= window.width || point.y < 0 || point.y >= window.height) continue;
		window.setPixelColour(point.x, point.y, colourInt, point.depth);
	}
}

void drawStrokedTriangle(DrawingWindow& window, CanvasTriangle* triangle, Colour colour) {
	drawLine(window, triangle->v0(), triangle->v1(), colour);
	drawLine(window, triangle->v1(), triangle->v2(), colour);
	drawLine(window, triangle->v2(), triangle->v0(), colour);
}

void drawFilledTriangle(DrawingWindow& window, CanvasTriangle* triangle, Colour colour) {
	float xMin = std::min(std::min(triangle->v0().x, triangle->v1().x), triangle->v2().x);
	float xMax = std::max(std::max(triangle->v0().x, triangle->v1().x), triangle->v2().x);
	float yMin = std::min(std::min(triangle->v0().y, triangle->v1().y), triangle->v2().y);
	float yMax = std::max(std::max(triangle->v0().y, triangle->v1().y), triangle->v2().y);
	u_int32_t colourInt = (255 << 24) + (int(colour.red) << 16) + (int(colour.green) << 8) + int(colour.blue);

	CanvasPoint p0 = triangle->v0();
	CanvasPoint p1 = triangle->v1();
	CanvasPoint p2 = triangle->v2();

	float signedArea = (p0.x - p2.x) * (p1.y - p2.y) - (p1.x - p2.x) * (p0.y - p2.y);
	if (signedArea == 0) return;
	if (signedArea < 0) {
		p0 = triangle->v1();
		p1 = triangle->v0();
		signedArea = -signedArea;
	}

	CanvasPoint v1 = CanvasPoint(p1.x - p0.x, p1.y - p0.y);
	CanvasPoint v2 = CanvasPoint(p2.x - p0.x, p2.y - p0.y);
	double dot11 = v1.x * v1.x + v1.y * v1.y;
	double dot12 = v1.x * v2.x + v1.y * v2.y;
	double dot22 = v2.x * v2.x + v2.y * v2.y;


	for (float x = xMin; x <= xMax; x++) {
		for (float y = yMin; y <= yMax; y++) {
			if (x < 0 || x >= window.width || y < 0 || y >= window.height) continue;
			CanvasPoint p = CanvasPoint(x, y);
			CanvasPoint v3 = CanvasPoint(p.x - p0.x, p.y - p0.y);
			double dot13 = v1.x * v3.x + v1.y * v3.y;
			double dot23 = v2.x * v3.x + v2.y * v3.y;
			double dot33 = v3.x * v3.x + v3.y * v3.y;

			double denom = (dot11 * dot22 - dot12 * dot12);
			double u = (dot22 * dot13 - dot12 * dot23) / denom;
			double v = (dot11 * dot23 - dot12 * dot13) / denom;

			float depth = p0.depth + u * (p1.depth - p0.depth) + v * (p2.depth - p0.depth);

			if (u >= 0.0f && v >= 0.0f && u + v < 1.0f) {
				window.setPixelColour(x, y, colourInt, depth);
			}
		}
	}
}

void drawTexturedTriangle(DrawingWindow& window, CanvasTriangle* triangle, Colour colour, TextureMap *texture, std::array<TexturePoint, 3> texturePoints) {
	float xMin = std::min(std::min(triangle->v0().x, triangle->v1().x), triangle->v2().x);
	float xMax = std::max(std::max(triangle->v0().x, triangle->v1().x), triangle->v2().x);
	float yMin = std::min(std::min(triangle->v0().y, triangle->v1().y), triangle->v2().y);
	float yMax = std::max(std::max(triangle->v0().y, triangle->v1().y), triangle->v2().y);

	CanvasPoint p0 = triangle->v0();
	CanvasPoint p1 = triangle->v1();
	CanvasPoint p2 = triangle->v2();

	float signedArea = (p0.x - p2.x) * (p1.y - p2.y) - (p1.x - p2.x) * (p0.y - p2.y);
	if (signedArea == 0) return;
	if (signedArea < 0) {
		p0 = triangle->v1();
		p1 = triangle->v0();
		signedArea = -signedArea;
	}

	CanvasPoint v1 = CanvasPoint(p1.x - p0.x, p1.y - p0.y);
	CanvasPoint v2 = CanvasPoint(p2.x - p0.x, p2.y - p0.y);
	double dot11 = v1.x * v1.x + v1.y * v1.y;
	double dot12 = v1.x * v2.x + v1.y * v2.y;
	double dot22 = v2.x * v2.x + v2.y * v2.y;
	double denom = (dot11 * dot22 - dot12 * dot12);

	for (float x = xMin; x <= xMax; x++) {
		for (float y = yMin; y <= yMax; y++) {
			if (x < 0 || x >= window.width || y < 0 || y >= window.height) continue;
			CanvasPoint p = CanvasPoint(x, y);
			CanvasPoint v3 = CanvasPoint(p.x - p0.x, p.y - p0.y);
			double dot13 = v1.x * v3.x + v1.y * v3.y;
			double dot23 = v2.x * v3.x + v2.y * v3.y;
			double dot33 = v3.x * v3.x + v3.y * v3.y;

			double u = (dot22 * dot13 - dot12 * dot23) / denom;
			double v = (dot11 * dot23 - dot12 * dot13) / denom;

			if (u >= 0.0f && v >= 0.0f && u + v < 1.0f) {
				float depth = p0.depth + u * (p1.depth - p0.depth) + v * (p2.depth - p0.depth);
				float textureX = texturePoints[0].x + u * (texturePoints[1].x - texturePoints[0].x) + v * (texturePoints[2].x - texturePoints[0].x);
				float textureY = texturePoints[0].y + u * (texturePoints[1].y - texturePoints[0].y) + v * (texturePoints[2].y - texturePoints[0].y);
				int index = std::max(0, std::min(int(textureY * texture->height), int(texture->height - 1))) * texture->width + std::max(0, std::min(int(textureX * texture->width), int(texture->width - 1)));
				u_int32_t textureColour = texture->pixels[index];
				window.setPixelColour(x, y, textureColour, depth);
			}
		}
	}
}

void drawTriangle3D(DrawingWindow& window, Camera& camera, ModelTriangle *triangle, bool wireframe) {
	Colour colour = triangle->colour;

	glm::vec3 vertex0 = triangle->vertices[0];
	glm::vec3 vertex1 = triangle->vertices[1];
	glm::vec3 vertex2 = triangle->vertices[2];

	CanvasPoint canvasPoint0 = camera.getCanvasIntersectionPoint(vertex0);
	CanvasPoint canvasPoint1 = camera.getCanvasIntersectionPoint(vertex1);
	CanvasPoint canvasPoint2 = camera.getCanvasIntersectionPoint(vertex2);

	CanvasTriangle canvasTriangle = CanvasTriangle(canvasPoint0, canvasPoint1, canvasPoint2);

	if (wireframe) drawStrokedTriangle(window, &canvasTriangle, colour);
	else if (triangle->textureMap.pixels.size() > 0) drawTexturedTriangle(window, &canvasTriangle, colour, &triangle->textureMap, triangle->texturePoints);
	else drawFilledTriangle(window, &canvasTriangle, colour);
}

void Draw::drawSceneRasterised() {
	for (size_t i = 0; i < scene.objects.size(); i++) {
		for (size_t j = 0; j < scene.objects.at(i).triangles.size(); j++) {
			drawTriangle3D(window, scene.camera, &scene.objects.at(i).triangles.at(j), false);
		}
	}
}

void Draw::drawSceneWireFrame() {
	for (size_t i = 0; i < scene.objects.size(); i++) {
		for (size_t j = 0; j < scene.objects.at(i).triangles.size(); j++) {
			drawTriangle3D(window, scene.camera, &scene.objects.at(i).triangles.at(j), true);
		}
	}
}