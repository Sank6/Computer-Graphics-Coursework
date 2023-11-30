#pragma once

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
#include "../scene/Camera.h"
#include "../scene/Scene.h"

enum Mode { WIREFRAME, RASTERISED, RAYTRACED };

class Draw {
public:
	DrawingWindow &window;
	Scene &scene;
	Draw(DrawingWindow &window, Scene &scene);
	void drawSceneRasterised();
	void drawSceneWireFrame();
	void drawSceneRayTraced();
};