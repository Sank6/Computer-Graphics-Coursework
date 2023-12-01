#pragma once

#include "Object.h"
#include <glm/glm.hpp>

class Light : public GenericObject {
public:
	float intensity;
    float radius;
	bool state;
	
	Light();
	Light(DrawingWindow &window, float intensitys);
};