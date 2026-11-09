#include "Curve.h"
#include <cmath>
#include "Line.h"

static PointF mix(PointF a, PointF b, float t) {
    return {a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t};
}

static PointF toF(Point p) {
    return {static_cast<float>(p.x), static_cast<float>(p.y)};
}

static Point toI(PointF p) {
    return {static_cast<int>(std::lround(p.x)), static_cast<int>(std::lround(p.y))};
}

static PointF quadAt(PointF p0, PointF p1, PointF p2, float t) {
    return mix(mix(p0, p1, t), mix(p1, p2, t), t);
}

static PointF cubicAt(PointF p0, PointF p1, PointF p2, PointF p3, float t) {
    return mix(quadAt(p0, p1, p2, t), quadAt(p1, p2, p3, t), t);
}

template <typename Eval>
static void sampleCurve(Canvas& canvas, Eval eval, Color color, int thickness, int segments) {
    Point prev = toI(eval(0.0f));
    for (int i = 1; i <= segments; ++i) {
        Point next = toI(eval(i / static_cast<float>(segments)));
        drawThickLine(canvas, prev.x, prev.y, next.x, next.y, thickness, color);
        prev = next;
    }
}

void drawQuadBezier(Canvas& canvas, Point p0, Point p1, Point p2,
                    Color color, int thickness, int segments) {
    PointF a = toF(p0), b = toF(p1), c = toF(p2);
    sampleCurve(canvas, [&](float t) { return quadAt(a, b, c, t); }, color, thickness, segments);
}

void drawCubicBezier(Canvas& canvas, Point p0, Point p1, Point p2, Point p3,
                     Color color, int thickness, int segments) {
    PointF a = toF(p0), b = toF(p1), c = toF(p2), d = toF(p3);
    sampleCurve(canvas, [&](float t) { return cubicAt(a, b, c, d, t); }, color, thickness, segments);
}
