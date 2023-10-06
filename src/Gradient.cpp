#include <CanvasTriangle.h>
#include <DrawingWindow.h>
#include <Utils.h>

#include <glm/glm.hpp>
#include <fstream>

#include "util/lerp.h"

#define WIDTH 640
#define HEIGHT 480

void drawGreyscale(DrawingWindow& window) {
    window.clearPixels();
    for (size_t y = 0; y < window.height; y++) {
        std::vector<float> values = interpolateSingleFloats(255.0f, 0.0f, window.width);
        for (size_t x = 0; x < window.width; x++) {
			float red = values[x];
			float green = values[x];
			float blue = values[x];
			uint32_t colour = (255 << 24) + (int(red) << 16) + (int(green) << 8) + int(blue);
			window.setPixelColour(x, y, colour);
		}
    }
}

void drawColour(DrawingWindow& window) {
    window.clearPixels();
    glm::vec3 topLeft(255, 0, 0);        // red 
    glm::vec3 topRight(0, 0, 255);       // blue 
    glm::vec3 bottomRight(0, 255, 0);    // green 
    glm::vec3 bottomLeft(255, 255, 0);   // yellow

    std::vector<glm::vec3> firstColumn = interpolateThreeElementValues(topLeft, bottomLeft, window.height);
    std::vector<glm::vec3> lastColumn = interpolateThreeElementValues(topRight, bottomRight, window.height);

    for (size_t y = 0; y < window.height; y++) {
		std::vector<glm::vec3> row = interpolateThreeElementValues(firstColumn[y], lastColumn[y], window.width);
        for (size_t x = 0; x < window.width; x++) {
			glm::vec3 colour = row[x];
			uint32_t colourInt = (255 << 24) + (int(colour.x) << 16) + (int(colour.y) << 8) + int(colour.z);
			window.setPixelColour(x, y, colourInt);
		}
	}
}

void handleEvent(SDL_Event event, DrawingWindow& window) {
    if (event.type == SDL_KEYDOWN) {
        if (event.key.keysym.sym == SDLK_LEFT)
            std::cout << "LEFT" << std::endl;
        else if (event.key.keysym.sym == SDLK_RIGHT)
            std::cout << "RIGHT" << std::endl;
        else if (event.key.keysym.sym == SDLK_UP)
            std::cout << "UP" << std::endl;
        else if (event.key.keysym.sym == SDLK_DOWN)
            std::cout << "DOWN" << std::endl;
    }
    else if (event.type == SDL_MOUSEBUTTONDOWN) {
        window.savePPM("output.ppm");
        window.saveBMP("output.bmp");
    }
}

int main(int argc, char* argv[]) {
    DrawingWindow window = DrawingWindow(WIDTH, HEIGHT, false);
    SDL_Event event;
    while (true) {
        // We MUST poll for events - otherwise the window will freeze !
        if (window.pollForInputEvents(event)) handleEvent(event, window);
        drawColour(window);
        // Need to render the frame at the end, or nothing actually gets shown on the screen !
        window.renderFrame();
    }
}
