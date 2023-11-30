#include "Object3d.h"

Object3d::Object3d() : GenericObject(){};

Object3d::Object3d(std::string name, DrawingWindow& window) : GenericObject(window) {
  this->name = name;
  this->shading = FLAT;
  this->textureMapping = false;
  this->triangles = std::vector<ModelTriangle>();

  this->reflectiveness = 0.2f;
  this->specularStrength = 0.1f;
  this->refractiveIndex = 1.0f;
  this->transparency = 0.0f;
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

void Object3d::updateTriangleNormals() {
  for (size_t i = 0; i < this->triangles.size(); i++) {
    ModelTriangle& triangle = this->triangles[i];
    for (size_t k = 0; k < 3; k++) {
      glm::vec3 vertex = triangle.vertices[k];
      glm::vec3 normal = glm::vec3(0, 0, 0);
      for (size_t l = 0; l < this->triangles.size(); l++) {
        ModelTriangle otherTriangle = this->triangles[l];
        if (otherTriangle.vertices[0] == vertex || otherTriangle.vertices[1] == vertex || otherTriangle.vertices[2] == vertex) {
          normal += otherTriangle.normal;
        }
      }
      triangle.vertexNormals[k] = glm::normalize(normal);
    }
    this->triangles[i] = triangle;
  }
}

void Object3d::updateTriangles() {
  for (size_t i = 0; i < this->triangles.size(); i++) {
    ModelTriangle& triangle = this->triangles[i];
    for (size_t j = 0; j < 3; j++) {
      triangle.transformedVertices[j] = glm::vec3(this->transformation * glm::vec4(triangle.vertices[j], 1.0f));
    }
  }
  this->updateBoundingBox();
  this->updateTriangleNormals();
}