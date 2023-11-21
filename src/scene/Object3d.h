#pragma once

#include "Object.h"
#include "ModelTriangle.h"

#include <vector>

enum Shading { FLAT, PHONG, GOUARD };

struct BoundingBox {
    glm::vec3 min;
    glm::vec3 max;
};

class Object3d : public GenericObject {
public:
    BoundingBox boundingBox;
    std::vector<ModelTriangle> triangles;
    Shading shading;
    bool textureMapping;

    Object3d(DrawingWindow &window);

    void addTriangle(ModelTriangle);
    void updateBoundingBox();
};
