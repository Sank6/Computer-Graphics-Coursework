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

#include "../util/util.h"
#include "Camera.h"

class Draw {
private:
	DrawingWindow &window;
	Camera &camera;
	std::vector<ModelTriangle> scene;
public:
	Draw(DrawingWindow &window, Camera &camera);
	void loadModel(std::string);
	void clearScene();
	void drawSceneRasterised();
	void drawSceneWireFrame();
	void drawSceneRayTraced();
};