#include "GiftBox.h"
#include <algorithm>
#include "../algorithms/Ellipse.h"
#include "../algorithms/Fill.h"
#include "../transform/Transform2D.h"

// Every part is a polygon in local coordinates, with (0, 0) at the bottom
// centre of the box. One matrix rotates them all about the box centre and
// moves them to (x, y); a squash then scales the result about the point
// where it touches the floor. Scanline fill draws each part.
void drawGiftBox(Canvas& canvas, int x, int y, int size, float angle, float squash) {
    const float half = size / 2.0f;
    const float boxH = size * 0.75f;
    const float lidH = std::max(2.0f, size / 5.0f);
    const float lidOver = size / 16.0f;
    const float ribbon = std::max(2.0f, size / 7.0f);
    const float top = boxH + lidH;

    Matrix3 m = translate(static_cast<float>(x), static_cast<float>(y)) * rotateAbout(angle, {0.0f, top / 2});
    const float sx = 1.0f + 0.5f * squash, sy = 1.0f - 0.72f * squash;
    if (squash > 0.0f) {
        // Flatten towards the lowest corner of the box, so it stays on the floor
        float floorY = 1e9f;
        for (PointF c : {PointF{-half, 0}, PointF{half, 0}, PointF{half, boxH}, PointF{-half, boxH}})
            floorY = std::min(floorY, apply(m, c).y);
        m = scaleAbout(sx, sy, {static_cast<float>(x), floorY}) * m;
    }
    auto rect = [&](float x0, float y0, float x1, float y1) {
        return transformPolygon(m, {{x0, y0}, {x1, y0}, {x1, y1}, {x0, y1}});
    };

    scanlineFill(canvas, rect(-half, 0, half, boxH), Palette::GiftBox);
    scanlineFill(canvas, rect(-half - lidOver, boxH, half + lidOver, top), Palette::GiftLid);
    scanlineFill(canvas, rect(-ribbon / 2, 0, ribbon / 2, top), Palette::Ribbon);

    // Bow: two loops shaped like a bow tie, and a round knot in the middle
    const float rx = size / 5.0f, ry = size / 9.0f;
    for (float side : {-1.0f, 1.0f}) {
        scanlineFill(canvas,
                     transformPolygon(m, {{0, top + ry * 0.4f}, {side * rx * 1.8f, top + ry * 1.9f},
                                          {side * rx * 2.1f, top + ry * 0.9f}, {side * rx * 1.5f, top}}),
                     Palette::Ribbon);
    }
    Point knot = toPoint(apply(m, {0, top + ry * 0.4f}));
    const int knotR = std::max(2, size / 12);
    fillEllipse(canvas, knot.x, knot.y, std::max(1, static_cast<int>(knotR * sx)), std::max(1, static_cast<int>(knotR * sy)),
                Palette::RibbonShade);
}
