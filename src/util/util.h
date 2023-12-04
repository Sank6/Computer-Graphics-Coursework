#include <vector>
#include <glm/glm.hpp>
#include <iostream>
#include <array>
#include <TexturePoint.h>
#include <TextureMap.h>
#include <Colour.h>
#include "../scene/Object3d.h"

std::vector<float> interpolateSingleFloats(float, float, int);
std::vector<glm::vec3> interpolateThreeElementValues(glm::vec3, glm::vec3, float);
std::array<TexturePoint, 3> getTexture(std::string textureFileName);
uint32_t colourToInt(Colour colour);
Colour intToColour(uint32_t colour);
Colour gouraudShading(Colour a, Colour b, Colour c, float u, float v);
uint32_t brighten(uint32_t colour, float factor);
bool intersectsAABB(glm::vec3& rayOrigin, glm::vec3& rayDirection, BoundingBox& box);
uint32_t combine(uint32_t colour1, uint32_t colour2, float percentOfColour1);