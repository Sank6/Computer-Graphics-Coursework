#include <CanvasTriangle.h>
#include <DrawingWindow.h>
#include <Utils.h>
#include <TextureMap.h>

#include <glm/glm.hpp>
#include <fstream>

#include "util/lerp.h"
#include "util/lines.h"

#define WIDTH 320
#define HEIGHT 240

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
            CanvasPoint p1 = CanvasPoint(160, 10);
            CanvasPoint p2 = CanvasPoint(300, 230);
            CanvasPoint p3 = CanvasPoint(10, 150);
            CanvasTriangle triangle = CanvasTriangle(p1, p2, p3);

            Colour colour = Colour(rand() % 255, rand() % 255, rand() % 255);

            std::cout << triangle << std::endl;

            TextureMap texture = TextureMap("../../../texture.ppm");
            CanvasPoint t1 = CanvasPoint(195, 5);
            CanvasPoint t2 = CanvasPoint(395, 380);
            CanvasPoint t3 = CanvasPoint(65, 330);
            CanvasTriangle textureTriangle = CanvasTriangle(t1, t2, t3);

            drawTexturedTriangle(window, triangle, Colour(50, 50, 50), texture, textureTriangle);
            drawStrokedTriangle(window, triangle, Colour(255, 255, 255));
            //drawStrokedTriangle(window, triangle, Colour(0, 0, 255));
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
