#include "Circle.h"
#include "Line.h"

// Walks the second octant (from the top of the circle towards 45 degrees)
// and calls visit(x, y) for each midpoint step.
template <typename Visit>
static void midpointCircle(int radius, Visit visit) {
    int x = 0;
    int y = radius;
    int p = 1 - radius;  // decision parameter

    while (x <= y) {
        visit(x, y);
        ++x;
        if (p < 0) {
            p += 2 * x + 1;
        } else {
            --y;
            p += 2 * (x - y) + 1;
        }
    }
}

void drawCircle(Canvas& canvas, int cx, int cy, int radius, Color color) {
    midpointCircle(radius, [&](int x, int y) {
        canvas.setPixel(cx + x, cy + y, color);
        canvas.setPixel(cx - x, cy + y, color);
        canvas.setPixel(cx + x, cy - y, color);
        canvas.setPixel(cx - x, cy - y, color);
        canvas.setPixel(cx + y, cy + x, color);
        canvas.setPixel(cx - y, cy + x, color);
        canvas.setPixel(cx + y, cy - x, color);
        canvas.setPixel(cx - y, cy - x, color);
    });
}

void fillCircle(Canvas& canvas, int cx, int cy, int radius, Color color) {
    midpointCircle(radius, [&](int x, int y) {
        drawSpan(canvas, cx - x, cx + x, cy + y, color);
        drawSpan(canvas, cx - x, cx + x, cy - y, color);
        drawSpan(canvas, cx - y, cx + y, cy + x, color);
        drawSpan(canvas, cx - y, cx + y, cy - x, color);
    });
}
