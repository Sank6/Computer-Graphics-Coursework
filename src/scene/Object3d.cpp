#include "Object3d.h"

Object3d::Object3d(DrawingWindow &window) : GenericObject(window) {
    this->shading = FLAT;
    this->textureMapping = false;
    this->triangles = std::vector<ModelTriangle>();
}

void Object3d::addTriangle(ModelTriangle triangle) {
    this->triangles.push_back(triangle);
}

void Object3d::updateBoundingBox() {
    this->boundingBox.min = glm::vec3(FLT_MAX, FLT_MAX, FLT_MAX);
    this->boundingBox.max = glm::vec3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
    for (size_t i = 0; i < this->triangles.size(); i++) {
        ModelTriangle& triangle = this->triangles[i];
        for (size_t j = 0; j < 3; j++) {
            glm::vec3& vertex = triangle.vertices[j];
            if (vertex.x < this->boundingBox.min.x) this->boundingBox.min.x = vertex.x;
            if (vertex.y < this->boundingBox.min.y) this->boundingBox.min.y = vertex.y;
            if (vertex.z < this->boundingBox.min.z) this->boundingBox.min.z = vertex.z;
            if (vertex.x > this->boundingBox.max.x) this->boundingBox.max.x = vertex.x;
            if (vertex.y > this->boundingBox.max.y) this->boundingBox.max.y = vertex.y;
            if (vertex.z > this->boundingBox.max.z) this->boundingBox.max.z = vertex.z;
        }
    }
}