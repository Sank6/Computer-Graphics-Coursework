#include <vector>
#include <glm/glm.hpp>

std::vector<float> interpolateSingleFloats(float from, float to, float numberOfValues) {
  std::vector<float> values = {};
  values.reserve((long) numberOfValues);
    float step = (to - from) / (numberOfValues - 1);
  for (float i = 0; i < numberOfValues; i++) values.push_back(from + (step * i));
  return values;
}

std::vector<glm::vec3> interpolateThreeElementValues(glm::vec3 from, glm::vec3 to, float numberOfValues) {
  std::vector<glm::vec3> values = {};
  values.reserve((long) numberOfValues);
  glm::vec3 step = (to - from) / (numberOfValues - 1.0f);
  for (float i = 0; i < numberOfValues; i++) values.push_back(from + (step * i));
  return values;
}