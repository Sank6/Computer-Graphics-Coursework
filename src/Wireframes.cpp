#include "util/lerp.h"
#include "util/lines.h"
#include "util/objParser.h"
#include "util/draw3d.h"

#define WIDTH 640
#define HEIGHT 480

void drawLines(DrawingWindow& window) {
    window.clearPixels();
    drawLine(window, CanvasPoint(0, 0), CanvasPoint(WIDTH / 2, HEIGHT / 2), Colour(255, 255, 255));
    drawLine(window, CanvasPoint(WIDTH - 1, 0), CanvasPoint(WIDTH / 2, HEIGHT / 2), Colour(255, 255, 255));
    drawLine(window, CanvasPoint(WIDTH / 2, 0), CanvasPoint(WIDTH / 2, HEIGHT - 1), Colour(255, 255, 255));
    drawLine(window, CanvasPoint(WIDTH / 3, HEIGHT / 2), CanvasPoint(WIDTH * 2 / 3, HEIGHT / 2), Colour(255, 255, 255));
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

        else if (event.key.keysym.sym == SDLK_u) {
            std::vector<ModelTriangle> triangles = loadObjFile("../../../cornell-box.obj");
            for (int i = 0; i < triangles.size(); i++) {
				drawTriangle3D(window, triangles[i]);
			}
        }
        else if (event.key.keysym.sym == SDLK_d) {
            system("pwd");
        }

        else if (event.key.keysym.sym == SDLK_c) {
            window.clearPixels();
        }
    }
    else if (event.type == SDL_MOUSEBUTTONDOWN) {
        window.savePPM("output.ppm");
        window.saveBMP("output.bmp");
    }
}

int main(int argc, char* argv[]) {
    DrawingWindow window = DrawingWindow(WIDTH, HEIGHT, false);
    SDL_Event event;

    std::vector<CanvasTriangle> triangles;

    // drawLines(window);

    while (true) {
        // We MUST poll for events - otherwise the window will freeze !
        if (window.pollForInputEvents(event)) handleEvent(event, window);
        // Need to render the frame at the end, or nothing actually gets shown on the screen !
        window.renderFrame();
    }
}
