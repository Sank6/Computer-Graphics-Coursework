#pragma once

#include <glm/glm.hpp>

class Light {
public:
	glm::vec3 position;
	float intensity;
    float radius;
	bool state;
	
	Light();
	Light(glm::vec3 position, float intensity, bool state);
};