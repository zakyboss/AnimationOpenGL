#pragma once
#include "../core/Canvas.h"
#include "../core/Geometry.h"

// Bezier curves evaluated with de Casteljau's algorithm, sampled into short
// line segments. Used for hair, smiles, stems and petals.
void drawQuadBezier(Canvas& canvas, Point p0, Point p1, Point p2,
                    Color color, int thickness = 1, int segments = 24);

void drawCubicBezier(Canvas& canvas, Point p0, Point p1, Point p2, Point p3,
                     Color color, int thickness = 1, int segments = 32);
