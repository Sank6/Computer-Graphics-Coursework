#include <DrawingWindow.h>
#include <Colour.h>
#include <CanvasPoint.h>
#include <CanvasTriangle.h>
#include <TextureMap.h>
#include <ModelTriangle.h>
#include <RayTriangleIntersection.h>
#include <glm/glm.hpp>
#include <vector>
#include <fstream>
#include <unordered_map>
#include <random>
#include <thread>

#include "../util/util.h"
#include "Camera.h"

class Light {
public:
	glm::vec3 position;
	float intensity;
	bool state;
	
	Light();
	Light(glm::vec3 position, float intensity, bool state);
};

class Draw {
private:
	DrawingWindow &window;
	Camera &camera;
	std::vector<ModelTriangle> scene;
	std::vector<Light> lights;
public:
	Draw(DrawingWindow &window, Camera &camera);
	void loadModel(std::string);
	void addLight(glm::vec3, float);
	void clearScene();
	void drawSceneRasterised();
	void drawSceneWireFrame();
	void drawSceneRayTraced();
};