#include "lines.h"
#include "lerp.h"

#include <tuple>

std::vector<std::tuple<float, float>> pointsOnLine(CanvasPoint from, CanvasPoint to) {
	std::vector<std::tuple<float, float>> points = {};
	float numberOfSteps = std::max(abs(to.x - from.x), abs(to.y - from.y)) + 1;
	std::vector<float> linePointsX = interpolateSingleFloats(from.x, to.x, numberOfSteps);
	std::vector<float> linePointsY = interpolateSingleFloats(from.y, to.y, numberOfSteps);

	for (int i = 0; i < numberOfSteps; i++) {
		points.push_back(std::make_tuple(linePointsX[i], linePointsY[i]));
	}

	return points;
}

void drawLine(DrawingWindow& window, CanvasPoint from, CanvasPoint to, Colour colour) {
	u_int32_t colourInt = (255 << 24) + (int(colour.red) << 16) + (int(colour.green) << 8) + int(colour.blue);
	std::vector<std::tuple<float, float>> linePoints = pointsOnLine(from, to);

	for (std::tuple<float, float> point : linePoints) {
		window.setPixelColour(std::get<0>(point), std::get<1>(point), colourInt);
	}
}

void drawStrokedTriangle(DrawingWindow& window, CanvasTriangle triangle, Colour colour) {
	drawLine(window, triangle.v0(), triangle.v1(), colour);
	drawLine(window, triangle.v1(), triangle.v2(), colour);
	drawLine(window, triangle.v2(), triangle.v0(), colour);
}

void drawFilledTriangle(DrawingWindow& window, CanvasTriangle triangle, Colour colour) {
	float xMin = std::min(std::min(triangle.v0().x, triangle.v1().x), triangle.v2().x);
	float xMax = std::max(std::max(triangle.v0().x, triangle.v1().x), triangle.v2().x);
	float yMin = std::min(std::min(triangle.v0().y, triangle.v1().y), triangle.v2().y);
	float yMax = std::max(std::max(triangle.v0().y, triangle.v1().y), triangle.v2().y);
	u_int32_t colourInt = (255 << 24) + (int(colour.red) << 16) + (int(colour.green) << 8) + int(colour.blue);

	CanvasPoint p0 = triangle.v0();
	CanvasPoint p1 = triangle.v1();
	CanvasPoint p2 = triangle.v2();
	

	float signedArea = (p0.x - p2.x) * (p1.y - p2.y) - (p1.x - p2.x) * (p0.y - p2.y);
	if (signedArea == 0) return;
	if (signedArea < 0) {
		p0 = triangle.v1();
		p1 = triangle.v0();
		signedArea = -signedArea;
	}

	CanvasPoint v1 = CanvasPoint(p1.x - p0.x, p1.y - p0.y);
	CanvasPoint v2 = CanvasPoint(p2.x - p0.x, p2.y - p0.y);
	double dot11 = v1.x * v1.x + v1.y * v1.y;
	double dot12 = v1.x * v2.x + v1.y * v2.y;
	double dot22 = v2.x * v2.x + v2.y * v2.y;


	for (float x = xMin; x <= xMax; x++) {
		for (float y = yMin; y <= yMax; y++) {
			CanvasPoint p = CanvasPoint(x, y);
			/*
			float alpha = ((p1.y - p2.y) * (p.x - p2.x) + (p2.x - p1.x) * (p.y - p2.y)) / signedArea;
			float beta = ((p2.y - p0.y) * (p.x - p2.x) + (p0.x - p2.x) * (p.y - p2.y)) / signedArea;
			float gamma = 1.0f - alpha - beta;

			if (alpha >= 0 && beta >= 0 && gamma >= 0) {
				window.setPixelColour(x, y, colourInt);
			}
			*/

			// Barycentric coordinates
			CanvasPoint v3 = CanvasPoint(p.x - p0.x, p.y - p0.y);
			double dot13 = v1.x * v3.x + v1.y * v3.y;
			double dot23 = v2.x * v3.x + v2.y * v3.y;
			double dot33 = v3.x * v3.x + v3.y * v3.y;

			double denom = (dot11 * dot22 - dot12 * dot12);
			double u = (dot22 * dot13 - dot12 * dot23) / denom;
			double v = (dot11 * dot23 - dot12 * dot13) / denom;

			if (u >= 0.0f && v >= 0.0f && u + v < 1.0f) {
				window.setPixelColour(x, y, colourInt);
			}

			// Method 3
			/*
			float s = (p0.x - p2.x) * (p.y - p2.y) - (p0.y - p2.y) * (p.x - p2.x);
			float t = (p1.x - p0.x) * (p.y - p0.y) - (p1.y - p0.y) * (p.y - p0.x);

			bool c = ((s < 0) != (t < 0) && s != 0 && t != 0);
			float d = (p2.x - p1.x) * (p.y - p1.y) - (p2.y - p1.y) * (p.x - p1.x);
			bool e = d == 0 || (d < 0) == (s + t <= 0);
			if (!c && e) {
				window.setPixelColour(x, y, colourInt);
			}
			*/
		}
	}
}

void drawTexturedTriangle(DrawingWindow& window, CanvasTriangle triangle, Colour colour, TextureMap texture, CanvasTriangle textureTriangle) {
	float xMin = std::min(std::min(triangle.v0().x, triangle.v1().x), triangle.v2().x);
	float xMax = std::max(std::max(triangle.v0().x, triangle.v1().x), triangle.v2().x);
	float yMin = std::min(std::min(triangle.v0().y, triangle.v1().y), triangle.v2().y);
	float yMax = std::max(std::max(triangle.v0().y, triangle.v1().y), triangle.v2().y);
	u_int32_t colourInt = (255 << 24) + (int(colour.red) << 16) + (int(colour.green) << 8) + int(colour.blue);

	CanvasPoint p0 = triangle.v0();
	CanvasPoint p1 = triangle.v1();
	CanvasPoint p2 = triangle.v2();

	CanvasPoint t0 = textureTriangle.v0();
	CanvasPoint t1 = textureTriangle.v1();
	CanvasPoint t2 = textureTriangle.v2();

	float signedArea = (p0.x - p2.x) * (p1.y - p2.y) - (p1.x - p2.x) * (p0.y - p2.y);
	if (signedArea == 0) return;
	if (signedArea < 0) {
		p0 = triangle.v1();
		p1 = triangle.v0();
		signedArea = -signedArea;
	}

	CanvasPoint v1 = CanvasPoint(p1.x - p0.x, p1.y - p0.y);
	CanvasPoint v2 = CanvasPoint(p2.x - p0.x, p2.y - p0.y);
	double dot11 = v1.x * v1.x + v1.y * v1.y;
	double dot12 = v1.x * v2.x + v1.y * v2.y;
	double dot22 = v2.x * v2.x + v2.y * v2.y;


	for (float x = xMin; x <= xMax; x++) {
		for (float y = yMin; y <= yMax; y++) {
			CanvasPoint p = CanvasPoint(x, y);
			CanvasPoint v3 = CanvasPoint(p.x - p0.x, p.y - p0.y);
			double dot13 = v1.x * v3.x + v1.y * v3.y;
			double dot23 = v2.x * v3.x + v2.y * v3.y;
			double dot33 = v3.x * v3.x + v3.y * v3.y;

			double denom = (dot11 * dot22 - dot12 * dot12);
			double u = (dot22 * dot13 - dot12 * dot23) / denom;
			double v = (dot11 * dot23 - dot12 * dot13) / denom;

			if (u >= 0.0f && v >= 0.0f && u + v < 1.0f) {
				// Get the pixel colour from the texture
				float textureX = t0.x + u * (t1.x - t0.x) + v * (t2.x - t0.x);
				float textureY = t0.y + u * (t1.y - t0.y) + v * (t2.y - t0.y);
				uint32_t textureColour = texture.pixels[int(textureY) * texture.width + int(textureX)];

				window.setPixelColour(x, y, textureColour);
			}
		}
	}
}