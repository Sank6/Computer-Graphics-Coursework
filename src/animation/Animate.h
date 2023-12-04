#pragma once

#include <iostream>
#include <sstream>
#include <iomanip>

#include "../render/Draw.h"
#include "../scene/Scene.h"

#define OUTPUT_FOLDER "/home/sank/framesout/"

void rotateCam(Scene& scene, int frames, int currentFrame);
void moveLightLeft(Scene& scene, int frames, int currentFrame);
void moveLightRight(Scene& scene, int frames, int currentFrame);
void doNothing(Scene& scene, int frames, int currentFrame);
void moveCornellBoxesAndSphere(Scene& scene, int frames, int currentFrame);

void animate(Draw draw, int animationID, float frames, void (*animation)(Scene& scene, int frames, int currentFrame), Mode mode);

