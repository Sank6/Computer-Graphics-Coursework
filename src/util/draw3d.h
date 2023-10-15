#include <CanvasPoint.h>
#include <CanvasTriangle.h>
#include <DrawingWindow.h>
#include <ModelTriangle.h>
#include <glm/glm.hpp>

CanvasPoint getCanvasIntersectionPoint(glm::vec3, glm::vec3, float, int, int);
void drawTriangle3D(DrawingWindow&, ModelTriangle);