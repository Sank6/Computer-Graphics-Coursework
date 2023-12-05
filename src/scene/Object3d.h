#pragma once

#include "Object.h"
#include "ModelTriangle.h"

#include <vector>

enum Shading { FLAT, PHONG, GOURAUD };

struct BoundingBox {
    glm::vec3 min;
    glm::vec3 max;
};

class Object3d : public GenericObject {
public:
    BoundingBox boundingBox;
    std::vector<ModelTriangle> triangles;

    Shading shading;
    float reflectiveness;
    float specularStrength;
    float refractiveIndex;
    float transparency;

    std::string name;

    bool textureMapping;
    bool cull;

    Object3d();
    Object3d(std::string name, DrawingWindow &window);

    void addTriangle(ModelTriangle);
    void updateBoundingBox();
    void updateTriangleNormals();
    void updateTriangles();
};
