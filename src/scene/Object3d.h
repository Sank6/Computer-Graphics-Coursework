#include "Object.h"
#include "ModelTriangle.h"

#include <vector>

enum Shading { FLAT, PHONG, GOUARD };

class Object3d : public GenericObject {
public:
    std::vector<ModelTriangle> triangles;
    Shading shading;
    bool textureMapping;

    Object3d(DrawingWindow &window);

    void addTriangle(ModelTriangle);
};
