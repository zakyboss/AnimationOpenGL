#include "StreetLife.h"
#include <algorithm>
#include <cmath>
#include "../algorithms/Circle.h"
#include "../algorithms/Curve.h"
#include "../algorithms/Ellipse.h"
#include "../algorithms/Fill.h"
#include "../algorithms/Line.h"

void drawBird(Canvas& canvas, int x, int y, int size, float flap) {
    const int lift = static_cast<int>(size * (0.5f - flap));
    const int t = std::max(1, size / 7);
    drawQuadBezier(canvas, {x - size, y + lift}, {x - size / 2, y + size / 3 + lift / 2}, {x, y}, Palette::Bird, t, 10);
    drawQuadBezier(canvas, {x, y}, {x + size / 2, y + size / 3 + lift / 2}, {x + size, y + lift}, Palette::Bird, t, 10);
}

void drawCar(Canvas& canvas, int x, int y, float scale, Color body, bool headlights) {
    auto s = [&](float v) { return static_cast<int>(std::lround(v * scale)); };
    if (headlights)
        scanlineFill(canvas, {{x + s(84), y + s(34)}, {x + s(300), y + s(4)}, {x + s(300), y + s(60)}}, Palette::Beam);

    scanlineFill(canvas, {{x - s(88), y + s(12)}, {x + s(88), y + s(12)}, {x + s(92), y + s(42)}, {x - s(90), y + s(44)}}, body);
    scanlineFill(canvas, {{x - s(50), y + s(42)}, {x + s(40), y + s(42)}, {x + s(22), y + s(74)}, {x - s(36), y + s(74)}}, body);
    scanlineFill(canvas, {{x - s(42), y + s(46)}, {x - s(8), y + s(46)}, {x - s(8), y + s(68)}, {x - s(31), y + s(68)}},
                 Palette::CarWindow);
    scanlineFill(canvas, {{x - s(2), y + s(46)}, {x + s(32), y + s(46)}, {x + s(18), y + s(68)}, {x - s(2), y + s(68)}},
                 Palette::CarWindow);
    for (int wx : {-52, 54}) {
        fillCircle(canvas, x + s(wx), y + s(12), s(15), Palette::Tyre);
        fillCircle(canvas, x + s(wx), y + s(12), s(6), Palette::Metal);
    }
    fillEllipse(canvas, x + s(86), y + s(32), s(5), s(4), headlights ? Palette::Headlight : Palette::Kerb);
    fillEllipse(canvas, x - s(87), y + s(32), s(4), s(4), Palette::Red);
}

void drawCigarette(Canvas& canvas, int x, int y, float time) {
    drawThickLine(canvas, x, y, x + 12, y + 3, 3, Palette::Cigarette);
    fillCircle(canvas, x + 13, y + 3, 2, Palette::Ember);
    // Four puffs at different stages, each rising, drifting and growing
    for (int k = 0; k < 4; ++k) {
        float p = std::fmod(time * 0.5f + k * 0.25f, 1.0f);
        int px = x + 14 + static_cast<int>(10 * std::sin(p * 5 + k) + p * 12);
        int py = y + 8 + static_cast<int>(p * 70);
        int r = 2 + static_cast<int>(p * 7);
        if (p < 0.85f) fillCircle(canvas, px, py, r, Palette::Smoke);
    }
}
