#include "Object3d.h"
#include "Camera.h"
#include "Light.h"

#include <vector>
#include <unordered_map>
#include <glm/glm.hpp>

class Scene {
private:
	DrawingWindow &window;

public:
    std::vector<Object3d> objects;
    std::vector<Light> lights;
	Camera &camera;

	Scene(DrawingWindow &window, Camera &camera);
	void loadModel(std::string);
	void addLight(Light light);
    void addObject(Object3d object);
	void clearScene();
};