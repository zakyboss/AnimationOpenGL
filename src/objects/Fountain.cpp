#include "Fountain.h"
#include <cmath>
#include "../algorithms/Circle.h"
#include "../algorithms/Curve.h"
#include "../algorithms/Ellipse.h"
#include "../algorithms/Fill.h"

namespace {

float gDark = 0.0f;
Color dim(Color c) { return lerp(c, Palette::NightTop, gDark * 0.55f); }

// Point at t along a quadratic Bezier curve
Point onCurve(Point a, Point b, Point c, float t) {
    float u = 1 - t;
    return {static_cast<int>(u * u * a.x + 2 * u * t * b.x + t * t * c.x),
            static_cast<int>(u * u * a.y + 2 * u * t * b.y + t * t * c.y)};
}

}  // namespace

void drawFountain(Canvas& canvas, int x, int y, float time, float darkness) {
    gDark = darkness;

    // Basin: a stone wall with an oval rim, holding the water
    fillEllipse(canvas, x, y, 96, 22, dim(Palette::StoneDark));
    fillRect(canvas, x - 96, y, 193, 20, dim(Palette::Stone));
    fillEllipse(canvas, x, y + 20, 96, 22, dim(Palette::Stone));
    fillEllipse(canvas, x, y + 21, 84, 16, dim(Palette::Water));

    // Ripples spreading out where the water lands
    for (int side : {-1, 1}) {
        float p = std::fmod(time * 0.9f + (side > 0 ? 0.5f : 0.0f), 1.0f);
        int rx = 4 + static_cast<int>(16 * p), ry = 1 + static_cast<int>(4 * p);
        drawEllipse(canvas, x + side * 52, y + 21, rx, ry, dim(Palette::WaterLight));
    }

    // Pillar and top bowl
    fillRect(canvas, x - 9, y + 20, 18, 72, dim(Palette::StoneDark));
    fillEllipse(canvas, x, y + 94, 38, 10, dim(Palette::Stone));
    fillEllipse(canvas, x, y + 97, 30, 6, dim(Palette::Water));
    fillCircle(canvas, x, y + 104, 5, dim(Palette::Stone));

    // Water arcs from the top into the basin, with droplets travelling along them
    const Point top{x, y + 108};
    for (int side : {-1, 1}) {
        Point control{x + side * 45, y + 150}, land{x + side * 52, y + 24};
        drawQuadBezier(canvas, top, control, land, dim(Palette::Water), 3, 20);
        for (int k = 0; k < 5; ++k) {
            float p = std::fmod(time * 1.3f + k / 5.0f, 1.0f);
            Point d = onCurve(top, control, land, p);
            fillCircle(canvas, d.x, d.y, 2, dim(Palette::WaterLight));
        }
    }
}

void drawBathingBird(Canvas& canvas, int x, int y, float time) {
    // Every 2.2 s it dips its head into the water and shakes its wings
    float cycle = std::fmod(time, 2.2f);
    float dip = cycle < 0.5f ? std::sin(cycle / 0.5f * 3.14159f) : 0.0f;
    bool splashing = cycle > 0.35f && cycle < 1.1f;

    fillEllipse(canvas, x, y + 7, 10, 6, Palette::BirdBrown);            // body
    scanlineFill(canvas, {{x + 8, y + 9}, {x + 18, y + 13}, {x + 17, y + 7}}, Palette::BirdBrown);  // tail
    int hx = x - 9, hy = y + 13 - static_cast<int>(dip * 9);
    fillCircle(canvas, hx, hy, 4, Palette::BirdBrown);                    // head
    scanlineFill(canvas, {{hx - 3, hy + 1}, {hx - 9, hy - 1}, {hx - 3, hy - 2}}, Palette::Beak);
    fillCircle(canvas, hx - 1, hy + 1, 1, Palette::Black);                // eye

    // Wing: flutters quickly while splashing
    int flap = splashing ? static_cast<int>(5 * std::sin(time * 40)) : 0;
    fillEllipse(canvas, x + 2, y + 9 + flap / 2, 7, 3 + std::abs(flap) / 2, lerp(Palette::BirdBrown, Palette::Black, 0.25f));

    if (splashing) {
        for (int k = 0; k < 6; ++k) {
            float p = std::fmod(time * 3.0f + k / 6.0f, 1.0f);
            int sx = x - 6 + static_cast<int>((k - 2.5f) * 6 * p);
            int sy = y + 2 + static_cast<int>(16 * std::sin(p * 3.14159f));
            fillCircle(canvas, sx, sy, 1 + (k % 2), Palette::WaterLight);
        }
    }
}
