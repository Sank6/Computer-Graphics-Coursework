#include "Light.h"

Light::Light() = default;
Light::Light(DrawingWindow &window, float intensity): GenericObject(window) {
    this->intensity = intensity;
    this->state = true;
    this->radius = 0.0f;
}
