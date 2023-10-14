#include <CanvasPoint.h>
#include <DrawingWindow.h>
#include <ModelTriangle.h>
#include <glm/glm.hpp>

CanvasPoint getCanvasIntersectionPoint(glm::vec3, glm::vec3, float, int, int);
void drawLine3D(DrawingWindow& window, glm::vec3, glm::vec3, Colour);
void drawTriangle3D(DrawingWindow&, ModelTriangle);