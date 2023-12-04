#include "Animate.h"

std::string intToString(int num) {
    std::ostringstream oss;
    oss << std::setw(3) << std::setfill('0') << num;
    return oss.str();
}

void doNothing(Scene& scene, int frames, int currentFrame) {
    return;
}

void moveLightLeft(Scene& scene, int frames, int currentFrame) {
    float startpos = 0.0f;
    float endpos = -1.5f;
    float step = (endpos - startpos) / frames;
    for (Light& light : scene.lights) {
        light.translate(glm::vec3(step, 0.0f, 0.0f));
    }
}

void moveLightRight(Scene& scene, int frames, int currentFrame) {
    float startpos = -1.5f;
    float endpos = 1.5f;
    float step = (endpos - startpos) / frames;
    for (Light& light : scene.lights) {
        light.translate(glm::vec3(step, 0.0f, 0.0f));
    }
}

void rollout(Scene& scene, int frames, int currentFrame) {
    float startpos = 0.0f;
    float endpos = 0.2f;
    float step = (endpos - startpos) / frames;

    float angle = 2 * M_PI / frames;
    scene.camera.rotateAroundPoint(glm::vec3(0.0f), angle, 1);

    for (Object3d& object : scene.objects) {
        if (object.name == "tall_box" || object.name == "short_box") {
            object.translate(glm::vec3(0.0f, step, 0.0f));
            object.updateTriangles();
        } else if (object.name == "sphere") {
            object.translate(glm::vec3(0.0f, step * 2, 0.0f));
            object.updateTriangles();
        }
    }
    
    float camStart = 6.0f;
    float camEnd = 2.75f;
    float camStep = (camEnd - camStart) / frames;
    scene.camera.translate(glm::vec3(0.0f, 0.0f, camStep));
}

void rotateCam(Scene& scene, int frames, int currentFrame) {
    float angle = 2 * M_PI / frames;
    scene.camera.rotateAroundPoint(glm::vec3(0.0f), angle, 1);
}

void slowDisappearCornell(Scene& scene, int frames, int currentFrame) {
    float frac = (float) (currentFrame + 1) / frames;
    if (frac > 0.8f) scene.clearScene();
    else if (frac > 0.6f) scene.unloadObject("sphere");
    else if (frac > 0.4f) scene.unloadObject("tall_box");
    else if (frac > 0.2f) scene.unloadObject("short_box");
    
    float startpos = 2.75f;
    float endpos = 4.0f;
    float step = (endpos - startpos) / frames;
    scene.camera.translate(glm::vec3(0.0f, 0.0f, step));
}

void panOut(Scene& scene, int frames, int currentFrame) {
}

void animate(Draw draw, int animationID, float frames, void (*animation)(Scene& scene, int frames, int currentFrame), Mode mode) {
    SDL_Event event;
    for (size_t i = 0; i < frames; i++) {
        uint32_t start = SDL_GetTicks();

        if (draw.window.pollForInputEvents(event) && event.type == SDL_QUIT) break;

        draw.window.clearPixels();
        animation(draw.scene, frames, i);
        if (mode == WIREFRAME) draw.drawSceneWireFrame();
        else if (mode == RASTERISED) draw.drawSceneRasterised();
        else if (mode == RAYTRACED) draw.drawSceneRayTraced();
        draw.window.renderFrame();

        std::string filename = "animation" + intToString(animationID) + "-frame" + intToString(i) + ".ppm";
        std::string filepath = OUTPUT_FOLDER + filename;
        draw.window.savePPM(filepath);
        
        uint32_t end = SDL_GetTicks();
        uint32_t duration = end - start;

        std::cout << "Frame " << i << "/" << frames << " rendered in " << duration << "ms" << std::endl;
    }
}