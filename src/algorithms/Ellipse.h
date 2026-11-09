#pragma once
#include "../core/Canvas.h"

// Midpoint ellipse algorithm (two regions), mirrored into four quadrants.
void drawEllipse(Canvas& canvas, int cx, int cy, int rx, int ry, Color color);

// Filled ellipse: same midpoint points, joined with horizontal spans.
void fillEllipse(Canvas& canvas, int cx, int cy, int rx, int ry, Color color);
