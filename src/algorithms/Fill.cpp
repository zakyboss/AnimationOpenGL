#include "Fill.h"
#include <algorithm>
#include <cmath>
#include "Line.h"

void drawPolygon(Canvas& canvas, const std::vector<Point>& points, Color color) {
    size_t n = points.size();
    for (size_t i = 0; i < n; ++i) {
        const Point& a = points[i];
        const Point& b = points[(i + 1) % n];
        drawLineBresenham(canvas, a.x, a.y, b.x, b.y, color);
    }
}

void scanlineFill(Canvas& canvas, const std::vector<Point>& points, Color color) {
    if (points.size() < 3) return;

    int minY = points[0].y, maxY = points[0].y;
    for (const Point& p : points) {
        minY = std::min(minY, p.y);
        maxY = std::max(maxY, p.y);
    }
    minY = std::max(minY, 0);
    maxY = std::min(maxY, canvas.height() - 1);

    std::vector<int> crossings;
    size_t n = points.size();
    for (int y = minY; y <= maxY; ++y) {
        // Sample at the pixel centre so shared vertices are not counted twice.
        float scanY = y + 0.5f;
        crossings.clear();
        for (size_t i = 0; i < n; ++i) {
            const Point& a = points[i];
            const Point& b = points[(i + 1) % n];
            if (a.y == b.y) continue;  // horizontal edges add no crossings
            bool spans = (a.y <= scanY && b.y > scanY) || (b.y <= scanY && a.y > scanY);
            if (!spans) continue;
            float t = (scanY - a.y) / static_cast<float>(b.y - a.y);
            crossings.push_back(static_cast<int>(std::lround(a.x + t * (b.x - a.x))));
        }
        std::sort(crossings.begin(), crossings.end());
        for (size_t i = 0; i + 1 < crossings.size(); i += 2)
            drawSpan(canvas, crossings[i], crossings[i + 1], y, color);
    }
}

void fillRect(Canvas& canvas, int x, int y, int w, int h, Color color) {
    for (int row = y; row < y + h; ++row) drawSpan(canvas, x, x + w - 1, row, color);
}

// Both seed fills use an explicit stack instead of recursion, since large
// regions (skies, walls) would overflow the call stack.
template <typename ShouldFill>
static void seedFill(Canvas& canvas, int x, int y, Color fill, ShouldFill shouldFill) {
    std::vector<Point> stack;
    stack.push_back({x, y});
    while (!stack.empty()) {
        Point p = stack.back();
        stack.pop_back();
        if (!canvas.inBounds(p.x, p.y)) continue;
        if (!shouldFill(canvas.getPixel(p.x, p.y))) continue;
        canvas.setPixel(p.x, p.y, fill);
        stack.push_back({p.x + 1, p.y});
        stack.push_back({p.x - 1, p.y});
        stack.push_back({p.x, p.y + 1});
        stack.push_back({p.x, p.y - 1});
    }
}

void floodFill(Canvas& canvas, int x, int y, Color fill) {
    if (!canvas.inBounds(x, y)) return;
    Color target = canvas.getPixel(x, y);
    if (target == fill) return;
    seedFill(canvas, x, y, fill, [&](Color c) { return c == target; });
}

void boundaryFill(Canvas& canvas, int x, int y, Color fill, Color boundary) {
    seedFill(canvas, x, y, fill, [&](Color c) { return c != boundary && c != fill; });
}

void fillVerticalGradient(Canvas& canvas, int y0, int y1, Color bottom, Color top) {
    if (y1 <= y0) return;
    for (int y = y0; y <= y1; ++y) {
        float t = (y - y0) / static_cast<float>(y1 - y0);
        drawSpan(canvas, 0, canvas.width() - 1, y, lerp(bottom, top, t));
    }
}
