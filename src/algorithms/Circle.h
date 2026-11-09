#pragma once
#include "../core/Canvas.h"

// Midpoint circle algorithm: computes one octant and mirrors it 8 ways.
void drawCircle(Canvas& canvas, int cx, int cy, int radius, Color color);

// Filled circle: same midpoint points, joined with horizontal spans.
void fillCircle(Canvas& canvas, int cx, int cy, int radius, Color color);
