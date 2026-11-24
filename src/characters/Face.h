#pragma once
#include "../core/Canvas.h"
#include "Expression.h"

struct FaceStyle {
    Color skin;
    Color brow;
    Color mouth;
    bool lashes = false;
};

// The parts of a face that move from moment to moment.
struct FaceMotion {
    float gazeX = 0.0f, gazeY = 0.0f;  // -1 to 1: where the eyes look
    float mouth = 0.0f;                // above 0.3 the mouth is open (talking)
    float blink = 0.0f;                // above 0.5 the eyes are shut
};

// Draws the eyes, eyebrows, nose and mouth onto a head already filled at
// (cx, cy) with radius r. Every feature is placed as a fraction of r, so the
// same face works at any size.
void drawFace(Canvas& canvas, int cx, int cy, int r, Expression e, const FaceStyle& style,
              const FaceMotion& motion = {});
