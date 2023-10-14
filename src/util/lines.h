#include <CanvasPoint.h>
#include <Colour.h>
#include <DrawingWindow.h>
#include <CanvasTriangle.h>
#include <TextureMap.h>

void drawLine(DrawingWindow&, CanvasPoint, CanvasPoint, Colour);

void drawStrokedTriangle(DrawingWindow&, CanvasTriangle, Colour);
void drawFilledTriangle(DrawingWindow&, CanvasTriangle, Colour);
void drawTexturedTriangle(DrawingWindow&, CanvasTriangle, Colour, TextureMap, CanvasTriangle);