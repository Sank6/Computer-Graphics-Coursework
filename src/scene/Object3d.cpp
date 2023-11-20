#include "Object3d.h"

Object3d::Object3d(DrawingWindow &window) : GenericObject(window) {
    this->shading = FLAT;
    this->textureMapping = false;
    this->triangles = std::vector<ModelTriangle>();
}

void Object3d::addTriangle(ModelTriangle triangle) {
    this->triangles.push_back(triangle);
}