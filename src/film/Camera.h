#pragma once
#include "../core/Canvas.h"
#include "../core/Geometry.h"

// Moves the camera over a finished frame: zoom > 1 pushes in towards focus
// (which stays fixed on screen), and (shakeX, shakeY) jolts the picture, for
// a slammed door. Each output pixel is found by applying the inverse of the
// camera's scale and translation to it, and reading the frame there.
void applyCamera(Canvas& frame, float zoom, PointF focus, int shakeX = 0, int shakeY = 0);
