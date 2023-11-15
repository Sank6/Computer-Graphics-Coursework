#include "render/Draw.h"
#include <glm/gtc/matrix_transform.hpp>

#define WIDTH 640
#define HEIGHT 480

enum Mode { WIREFRAME, RASTERISED, RAYTRACED };
struct Fps { unsigned long average; unsigned long last;  unsigned long count; };

int main(int argc, char* argv[]) {
    DrawingWindow window = DrawingWindow(WIDTH, HEIGHT, false);
    SDL_Event event;

    std::vector<CanvasTriangle> triangles;

    Camera camera = Camera(window);
    Draw draw = Draw(window, camera);
    draw.loadModel("../../../cornell-box.obj");
    draw.addLight(glm::vec3(0.0f, 0.6f, 0.5f), 1.5f);

    float rotationSpeed = 0.01f;
    float translationSpeed = 0.1f;
    bool rotating = false;
    Mode mode = RAYTRACED;

    std::unordered_map<Mode, Fps> fpsMap;
    fpsMap[WIREFRAME] = Fps();
    fpsMap[RASTERISED] = Fps();
    fpsMap[RAYTRACED] = Fps();

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
                if (event.key.keysym.sym == SDLK_m && mode == RASTERISED) mode = RAYTRACED;
                else if (event.key.keysym.sym == SDLK_m && mode == RAYTRACED) mode = WIREFRAME;
                else if (event.key.keysym.sym == SDLK_m && mode == WIREFRAME) mode = RASTERISED;
                if (event.key.keysym.sym == SDLK_SPACE) rotating = !rotating;
                if (event.key.keysym.sym == SDLK_UP) camera.rotateAroundPoint(glm::vec3(0.0f), -rotationSpeed, X);
                if (event.key.keysym.sym == SDLK_DOWN) camera.rotateAroundPoint(glm::vec3(0.0f), rotationSpeed, X);
				if (event.key.keysym.sym == SDLK_LEFT) camera.rotateAroundPoint(glm::vec3(0.0f), -rotationSpeed, Y);
				if (event.key.keysym.sym == SDLK_RIGHT) camera.rotateAroundPoint(glm::vec3(0.0f), rotationSpeed, Y);
            }
        }

        if (mode == RAYTRACED) rotationSpeed = 0.1f;
        else rotationSpeed = 0.01f;
        
        if (rotating) camera.rotateAroundPoint(glm::vec3(0.0f), rotationSpeed, Y);

        window.clearPixels();
        if (mode == WIREFRAME) draw.drawSceneWireFrame();
        else if (mode == RASTERISED) draw.drawSceneRasterised();
        else if (mode == RAYTRACED) draw.drawSceneRayTraced();
        window.renderFrame();

        // Measure time taken to render scene
        uint32_t end = SDL_GetTicks();
        uint32_t duration = end - start;
        float fps = 1000.0f / duration;

        fpsMap[mode].last = fps;
        fpsMap[mode].average = (fpsMap[mode].average * fpsMap[mode].count + fps) / (fpsMap[mode].count + 1);
        fpsMap[mode].count++;

        // std::cout << "Mode: " << mode << " FPS: " << fps << " Average: " << fpsMap[mode].average << std::endl;
    }
}
