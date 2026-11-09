#pragma once
#include "../core/Canvas.h"

// Digital Differential Analyzer: steps along the major axis using floating point increments.
void drawLineDDA(Canvas& canvas, int x0, int y0, int x1, int y1, Color color);

// Bresenham: integer-only decision variable, works in all octants.
void drawLineBresenham(Canvas& canvas, int x0, int y0, int x1, int y1, Color color);

// Bresenham line drawn with a square brush, for limbs, outlines and roads.
void drawThickLine(Canvas& canvas, int x0, int y0, int x1, int y1, int thickness, Color color);

// Horizontal run of pixels from x0 to x1 (inclusive). Used by the fill routines.
void drawSpan(Canvas& canvas, int x0, int x1, int y, Color color);
