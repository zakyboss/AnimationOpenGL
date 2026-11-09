#pragma once
#include "../core/Canvas.h"
#include "Expression.h"

struct FaceStyle {
    Color skin;
    Color brow;
    Color mouth;
    bool lashes = false;
};

// Draws the eyes, eyebrows, nose and mouth onto a head already filled at
// (cx, cy) with radius r. Every feature is placed as a fraction of r, so the
// same face works at any size.
void drawFace(Canvas& canvas, int cx, int cy, int r, Expression e, const FaceStyle& style);
