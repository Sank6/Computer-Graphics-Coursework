#include "Light.h"

Light::Light() = default;
Light::Light(glm::vec3 position, float intensity, bool state): position(position), intensity(intensity), state(state) {}
