#include "render/Draw.h"
#include <glm/gtc/matrix_transform.hpp>

#define WIDTH 640
#define HEIGHT 480

int main(int argc, char* argv[]) {
    DrawingWindow window = DrawingWindow(WIDTH, HEIGHT, false);
    SDL_Event event;

    std::vector<CanvasTriangle> triangles;

    Camera camera = Camera(window);
    Draw draw = Draw(window, camera);
    draw.loadModel("../../../cornell-box.obj");


    float rotationSpeed = 0.01f;

    float translationSpeed = 0.1f;
    bool rotating = false;
    bool wireframe = false;

    while (true) {
        // Measure time taken to render scene
        uint32_t start = SDL_GetTicks();

        if (window.pollForInputEvents(event)) {
			if (event.type == SDL_QUIT) break;
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_a) camera.translate(glm::vec3(-translationSpeed, 0, 0));
                if (event.key.keysym.sym == SDLK_d) camera.translate(glm::vec3(translationSpeed, 0, 0));
                if (event.key.keysym.sym == SDLK_w) camera.translate(glm::vec3(0, -translationSpeed, 0));
                if (event.key.keysym.sym == SDLK_s) camera.translate(glm::vec3(0, translationSpeed, 0));
                if (event.key.keysym.sym == SDLK_q) camera.translate(glm::vec3(0, 0, -translationSpeed));
                if (event.key.keysym.sym == SDLK_e) camera.translate(glm::vec3(0, 0, translationSpeed));
                if (event.key.keysym.sym == SDLK_r) camera.reset();
                if (event.key.keysym.sym == SDLK_f) wireframe = !wireframe;
                if (event.key.keysym.sym == SDLK_SPACE) rotating = !rotating;
                if (event.key.keysym.sym == SDLK_UP) camera.rotateX(-rotationSpeed);
                if (event.key.keysym.sym == SDLK_DOWN) camera.rotateX(rotationSpeed);
				if (event.key.keysym.sym == SDLK_LEFT) camera.rotateY(-rotationSpeed);
				if (event.key.keysym.sym == SDLK_RIGHT) camera.rotateY(rotationSpeed);
            }
        }

        draw.drawScene(wireframe);
        if (rotating) camera.rotateY(rotationSpeed);

        window.renderFrame();

        // Measure time taken to render scene
        uint32_t end = SDL_GetTicks();
        uint32_t duration = end - start;
        float fps = 1000.0f / duration;
        // std::cout << "FPS: " << fps << std::endl;
    }
}
