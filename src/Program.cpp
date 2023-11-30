#include "render/Draw.h"
#include "animation/Animate.h"

#define WIDTH 960
#define HEIGHT 720

struct Fps { float average; float last;  unsigned long count; };

int main(int argc, char* argv[]) {
    DrawingWindow window = DrawingWindow(WIDTH, HEIGHT, false);
    SDL_Event event;

    Camera camera = Camera(window);
	camera.translate(glm::vec3(0.0f, 0.0f, 4.0f));
    camera.setStartPosition();
    
    Scene scene = Scene(window, camera);
    scene.loadModel("../../../combined.obj", 0.35f);
    // scene.loadModel("../../../bunny.obj", 1.2f);
    // Light light2 = Light(glm::vec3(-1.0f, 1.8f, 2.0f), 5.0f, true);
    // scene.addLight(light2);
    Light light = Light(glm::vec3(0.0f, 0.8f, 0.0f), 0.5f, true);
    scene.addLight(light);
    Draw draw = Draw(window, scene);

    float rotationSpeed = 0.1f;
    float translationSpeed = 0.1f;
    bool rotating = false;
    Mode mode = RAYTRACED;
    std::unordered_map<Mode, Fps> fpsMap;
    fpsMap[WIREFRAME] = Fps();
    fpsMap[RASTERISED] = Fps();
    fpsMap[RAYTRACED] = Fps();

    // animate(draw, 0, 120, rotateCam, RAYTRACED);
    // return 0;

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

                if (event.key.keysym.sym == SDLK_UP) camera.rotateAroundPoint(glm::vec3(0.0f), -rotationSpeed, 0);
                if (event.key.keysym.sym == SDLK_DOWN) camera.rotateAroundPoint(glm::vec3(0.0f), rotationSpeed, 0);
				if (event.key.keysym.sym == SDLK_LEFT) camera.rotateAroundPoint(glm::vec3(0.0f), -rotationSpeed, 1);
				if (event.key.keysym.sym == SDLK_RIGHT) camera.rotateAroundPoint(glm::vec3(0.0f), rotationSpeed, 1);

                if (event.key.keysym.sym == SDLK_z) scene.shadowPass = !scene.shadowPass;
                if (event.key.keysym.sym == SDLK_x) scene.specularPass = !scene.specularPass;
                if (event.key.keysym.sym == SDLK_c) scene.falloffPass = !scene.falloffPass;
                if (event.key.keysym.sym == SDLK_v) scene.aoiPass = !scene.aoiPass;
                if (event.key.keysym.sym == SDLK_b) scene.ambientPass = !scene.ambientPass;
                if (event.key.keysym.sym == SDLK_n) scene.reflectionPass = !scene.reflectionPass;
                if (event.key.keysym.sym == SDLK_COMMA) scene.refractionPass = !scene.refractionPass;
            }
        }
        
        if (rotating) camera.rotateAroundPoint(glm::vec3(0.0f), rotationSpeed * 0.5, 1);

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

        std::cout << "Mode: " << mode << " FPS: " << fps << " Average: " << fpsMap[mode].average << std::endl;
    }
}
