#include "Object3d.h"
#include "Camera.h"
#include "Light.h"

#include <vector>
#include <unordered_map>
#include<sstream>
#include <glm/glm.hpp>

class Scene {
private:
	DrawingWindow &window;

public:
    std::vector<Object3d> objects;
    std::vector<Light> lights;
	Camera &camera;

	// Render Passes
	bool shadowPass;
	bool specularPass;
	bool falloffPass;
	bool aoiPass;
	bool ambientPass;
	bool reflectionPass;

	Colour backgroundColour;

	Scene(DrawingWindow &window, Camera &camera);
	void loadModel(std::string, float scalingFactor = 0.35f);
	void addLight(Light light);
    void addObject(Object3d object);
	void clearScene();
};