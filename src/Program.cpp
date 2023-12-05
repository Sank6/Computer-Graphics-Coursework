#include "render/Draw.h"
#include "animation/Animate.h"

#include <unistd.h>
#include <stdio.h>
#include <limits.h>

#define WIDTH 320
#define HEIGHT 240

#define ASSETS_FOLDER "./assets/"

struct Fps { float average; float last;  unsigned long count; };

int main(int argc, char* argv[]) {
    DrawingWindow window = DrawingWindow(WIDTH, HEIGHT, false);
    SDL_Event event;

    Camera camera = Camera(window);
	camera.translate(glm::vec3(0.0f, 0.0f, 6.0f));
    camera.setStartPosition();
    
    Scene scene = Scene(window, camera);

    Light cornellLight = Light(window, 1.7f);
    cornellLight.setPosition(glm::vec3(0.0f, 0.6f, 1.5f));

    Light bunnyLight = Light(window, 1.7f);
    bunnyLight.setPosition(glm::vec3(0.0f, 0.6f, 1.5f));

    Draw draw = Draw(window, scene);

    float rotationSpeed = 0.1f;
    float translationSpeed = 0.1f;
    bool rotating = false;
    Mode mode = RAYTRACED;
    std::unordered_map<Mode, Fps> fpsMap;
    fpsMap[WIREFRAME] = Fps();
    fpsMap[RASTERISED] = Fps();
    fpsMap[RAYTRACED] = Fps();

    std::string command = "rm -rf " OUTPUT_FOLDER "*";
    int out = system(command.c_str());
    int multiplier = 24;

    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        std::cout << "Current working dir: " << cwd << std::endl;
    } else {
        perror("getcwd() error");
        return 1;
    }
    
    scene.addLight(cornellLight);
    scene.addEnvironmentMap(ASSETS_FOLDER "space.ppm");
    scene.loadModel(ASSETS_FOLDER "cornell.obj", 0.35f);
    animate(draw, 1, 1 * multiplier, doNothing, RASTERISED);
    animate(draw, 0, 1 * multiplier, doNothing, WIREFRAME);
    animate(draw, 3, 4 * multiplier, rollout, RAYTRACED);
    animate(draw, 4, 2 * multiplier, slowDisappearCornell, RAYTRACED);
    scene.lights.clear();
    scene.addLight(bunnyLight);
    scene.loadModel(ASSETS_FOLDER "sphere.obj", 0.35f);
    scene.objects[0].shading = GOURAUD;
    animate(draw, 5, 1 * multiplier, moveLightLeft, RAYTRACED);
    animate(draw, 6, 2 * multiplier, moveLightRight, RAYTRACED);
    animate(draw, 7, 1 * multiplier, moveLightLeft, RAYTRACED);
    scene.objects[0].shading = PHONG;
    animate(draw, 8, 1 * multiplier, moveLightLeft, RAYTRACED);
    animate(draw, 9, 2 * multiplier, moveLightRight, RAYTRACED);
    animate(draw, 10, 1 * multiplier, moveLightLeft, RAYTRACED);
    scene.objects[0].reflectiveness = 1.0f;
    scene.addEnvironmentMap(ASSETS_FOLDER "vatican.ppm");
    animate(draw, 11, 4 * multiplier, rotateCam, RAYTRACED);
    scene.objects.clear();
    scene.loadModel(ASSETS_FOLDER "bunny.obj", 0.8f);
    scene.objects[0].shading = PHONG;
    scene.objects[0].transparency = 0.8f;
    scene.objects[0].refractiveIndex = 1.5f;
    scene.addEnvironmentMap(ASSETS_FOLDER "christmas_room.ppm");
    animate(draw, 12, 4 * multiplier, rotateCam, RAYTRACED);
    return 0;

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
