#pragma once
#include <vector>
#include "../core/Canvas.h"
#include "../core/Geometry.h"

// Outline of a closed polygon using Bresenham lines.
void drawPolygon(Canvas& canvas, const std::vector<Point>& points, Color color);

// Scanline polygon fill (even-odd rule). Works for convex and concave polygons.
void scanlineFill(Canvas& canvas, const std::vector<Point>& points, Color color);

// Axis-aligned filled rectangle; (x, y) is the bottom-left corner.
void fillRect(Canvas& canvas, int x, int y, int w, int h, Color color);

// 4-connected flood fill: replaces the connected region of the seed's colour.
void floodFill(Canvas& canvas, int x, int y, Color fill);

// 4-connected boundary fill: fills outward from the seed until it hits boundary colour.
void boundaryFill(Canvas& canvas, int x, int y, Color fill, Color boundary);

// Vertical gradient between two colours, for skies and fades.
void fillVerticalGradient(Canvas& canvas, int y0, int y1, Color bottom, Color top);
