#include <vector>
#include <glm/glm.hpp>
#include <iostream>
#include <array>
#include <TexturePoint.h>
#include <TextureMap.h>

std::vector<float> interpolateSingleFloats(float, float, int);
std::vector<glm::vec3> interpolateThreeElementValues(glm::vec3, glm::vec3, float);
std::array<TexturePoint, 3> getTexture(std::string textureFileName);