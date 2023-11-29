#include "util.h"

std::vector<float> interpolateSingleFloats(float from, float to, int numberOfValues) {
	std::vector<float> values = {};
	values.reserve(numberOfValues);
	float step = (to - from) / (numberOfValues - 1);
	for (int i = 0; i < numberOfValues; i++) values.push_back(from + (step * i));
	return values;
}

std::vector<glm::vec3> interpolateThreeElementValues(glm::vec3 from, glm::vec3 to, float numberOfValues) {
  std::vector<glm::vec3> values = {};
  values.reserve((long) numberOfValues);
  glm::vec3 step = (to - from) / (numberOfValues - 1.0f);
  for (float i = 0; i < numberOfValues; i++) values.push_back(from + (step * i));
  return values;
}

std::array<TexturePoint, 3> getTexture(std::string textureFileName) {
	std::array<TexturePoint, 3> texturePoints;
	TextureMap textureMap = TextureMap(textureFileName);
	return texturePoints;
}

uint32_t colourToInt(Colour colour) {
  return (255 << 24) + (int(colour.red) << 16) + (int(colour.green) << 8) + int(colour.blue);
}

Colour intToColour(uint32_t colour) {
  return Colour("Colour", (colour >> 16) & 255, (colour >> 8) & 255, colour & 255);
}


Colour gouradShading(Colour a, Colour b, Colour c, float u, float v) {
  float w = 1.0f - u - v;
  float red = a.red * w + b.red * u + c.red * v;
  float green = a.green * w + b.green * u + c.green * v;
  float blue = a.blue * w + b.blue * u + c.blue * v;
  return Colour("Colour", red, green, blue);
}

uint32_t brighten(uint32_t colour, float factor) {
  uint32_t red = (colour >> 16) & 255;
  uint32_t green = (colour >> 8) & 255;
  uint32_t blue = colour & 255;
  red *= factor;
  green *= factor;
  blue *= factor;
  red = std::max(0, std::min(255, (int)red));
  green = std::max(0, std::min(255, (int)green));
  blue = std::max(0, std::min(255, (int)blue));
  return (255 << 24) + (red << 16) + (green << 8) + blue;
}

bool intersectsAABB(glm::vec3& rayOrigin, glm::vec3& rayDirection, BoundingBox& box) {
  glm::vec3 dirFrac = glm::vec3(1.0f / rayDirection.x, 1.0f / rayDirection.y, 1.0f / rayDirection.z);

  float i1 = (box.min.x - rayOrigin.x) * dirFrac.x;
  float i2 = (box.max.x - rayOrigin.x) * dirFrac.x;
  float i3 = (box.min.y - rayOrigin.y) * dirFrac.y;
  float i4 = (box.max.y - rayOrigin.y) * dirFrac.y;
  float i5 = (box.min.z - rayOrigin.z) * dirFrac.z;
  float i6 = (box.max.z - rayOrigin.z) * dirFrac.z;

  float imin = std::max(std::max(std::min(i1, i2), std::min(i3, i4)), std::min(i5, i6));
  float imax = std::min(std::min(std::max(i1, i2), std::max(i3, i4)), std::max(i5, i6));

  if (imax < 0 || imin > imax)
    return false;
  else
    return true;
}


uint32_t combine(uint32_t colour1, uint32_t colour2, float percentOfColour1) {
  float percentOfColour2 = 1.0f - percentOfColour1;
  uint32_t red = ((colour1 >> 16) & 255) * percentOfColour1 + ((colour2 >> 16) & 255) * percentOfColour2;
  uint32_t green = ((colour1 >> 8) & 255) * percentOfColour1 + ((colour2 >> 8) & 255) * percentOfColour2;
  uint32_t blue = (colour1 & 255) * percentOfColour1 + (colour2 & 255) * percentOfColour2;
  return (255 << 24) + (red << 16) + (green << 8) + blue;
}