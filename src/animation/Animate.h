#pragma once

#include <iostream>
#include <sstream>
#include <iomanip>

#include "../render/Draw.h"
#include "../scene/Scene.h"

void rotateCam(Scene& scene);

void animate(Draw draw, int animationID, float frames, void (*animation)(Scene& scene), Mode mode);

