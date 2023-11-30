#include "Animate.h"

std::string intToString(int num) {
    std::ostringstream oss;
    oss << std::setw(3) << std::setfill('0') << num;
    return oss.str();
}

void moveSceneRight(Scene& scene) {
    scene.camera.translate(glm::vec3(-0.01f, 0.0f, 0.0f));
    scene.camera.rotateAroundPoint(glm::vec3(0.0f), 0.01f, 1);
}

void animate(Draw draw, int animationID, float frames, void (*animation)(Scene& scene), Mode mode) {
    for (size_t i = 0; i < frames; i++) {
        uint32_t start = SDL_GetTicks();

        draw.window.clearPixels();
        animation(draw.scene);
        if (mode == WIREFRAME) draw.drawSceneWireFrame();
        else if (mode == RASTERISED) draw.drawSceneRasterised();
        else if (mode == RAYTRACED) draw.drawSceneRayTraced();
        draw.window.renderFrame();

        std::string filename = "/home/sank/framesout/animation" + intToString(animationID) + "-frame" + intToString(i) + ".ppm";
        draw.window.savePPM(filename);
        
        uint32_t end = SDL_GetTicks();
        uint32_t duration = end - start;

        std::cout << "Frame " << i << "/" << frames << " rendered in " << duration << "ms" << std::endl;
    }
}