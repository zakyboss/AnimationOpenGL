#include "Line.h"
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <utility>

void drawLineDDA(Canvas& canvas, int x0, int y0, int x1, int y1, Color color) {
    int dx = x1 - x0;
    int dy = y1 - y0;
    int steps = std::max(std::abs(dx), std::abs(dy));
    if (steps == 0) {
        canvas.setPixel(x0, y0, color);
        return;
    }

    float xInc = dx / static_cast<float>(steps);
    float yInc = dy / static_cast<float>(steps);
    float x = static_cast<float>(x0);
    float y = static_cast<float>(y0);
    for (int i = 0; i <= steps; ++i) {
        canvas.setPixel(static_cast<int>(std::lround(x)), static_cast<int>(std::lround(y)), color);
        x += xInc;
        y += yInc;
    }
}

// Generic Bresenham that calls plot(x, y) for each pixel, so the thin and
// thick versions share the same rasterisation.
template <typename Plot>
static void bresenham(int x0, int y0, int x1, int y1, Plot plot) {
    int dx = std::abs(x1 - x0);
    int dy = -std::abs(y1 - y0);
    int sx = x0 < x1 ? 1 : -1;
    int sy = y0 < y1 ? 1 : -1;
    int err = dx + dy;

    while (true) {
        plot(x0, y0);
        if (x0 == x1 && y0 == y1) break;
        int e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
}

void drawLineBresenham(Canvas& canvas, int x0, int y0, int x1, int y1, Color color) {
    bresenham(x0, y0, x1, y1, [&](int x, int y) { canvas.setPixel(x, y, color); });
}

void drawThickLine(Canvas& canvas, int x0, int y0, int x1, int y1, int thickness, Color color) {
    if (thickness <= 1) {
        drawLineBresenham(canvas, x0, y0, x1, y1, color);
        return;
    }
    int lo = -(thickness / 2);
    int hi = lo + thickness - 1;
    bresenham(x0, y0, x1, y1, [&](int x, int y) {
        for (int oy = lo; oy <= hi; ++oy)
            for (int ox = lo; ox <= hi; ++ox)
                canvas.setPixel(x + ox, y + oy, color);
    });
}

void drawSpan(Canvas& canvas, int x0, int x1, int y, Color color) {
    if (x0 > x1) std::swap(x0, x1);
    for (int x = x0; x <= x1; ++x) canvas.setPixel(x, y, color);
}
