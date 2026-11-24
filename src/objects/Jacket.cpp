#include "Jacket.h"
#include <algorithm>
#include <cmath>
#include "../algorithms/Circle.h"
#include "../algorithms/Ellipse.h"
#include "../algorithms/Fill.h"
#include "../transform/Transform2D.h"

static int px(float v) { return static_cast<int>(std::lround(v)); }

void drawJacketBody(Canvas& canvas, const Skeleton& sk, bool fromBehind) {
    const float s = sk.scale;
    const int x = sk.neckBase.x, top = sk.neckBase.y;
    const int hem = sk.pelvis.y - px(14 * s);
    const int outerTopL = sk.shoulder[Left].x - sk.armRadius, outerTopR = sk.shoulder[Right].x + sk.armRadius;
    const int outerBotL = sk.hip[Left].x - sk.legRadius - px(8 * s), outerBotR = sk.hip[Right].x + sk.legRadius + px(8 * s);

    if (fromBehind) {
        scanlineFill(canvas, {{outerTopL, top}, {outerTopR, top}, {outerBotR, hem}, {outerBotL, hem}}, Palette::Jacket);
        fillRect(canvas, outerBotL, hem, outerBotR - outerBotL, std::max(2, px(5 * s)), Palette::JacketShade);
        return;
    }

    // Two front panels with a gap down the middle, and darker lapels at the neck
    const int gap = px(8 * s);
    scanlineFill(canvas, {{x - gap, top}, {outerTopL, top}, {outerBotL, hem}, {x - gap / 2, hem}}, Palette::Jacket);
    scanlineFill(canvas, {{x + gap, top}, {outerTopR, top}, {outerBotR, hem}, {x + gap / 2, hem}}, Palette::Jacket);
    scanlineFill(canvas, {{x - gap, top}, {x - gap - px(12 * s), top}, {x - gap, top - px(30 * s)}}, Palette::JacketShade);
    scanlineFill(canvas, {{x + gap, top}, {x + gap + px(12 * s), top}, {x + gap, top - px(30 * s)}}, Palette::JacketShade);
}

void drawJacketSleeves(Canvas& canvas, const Skeleton& sk, bool sleeves, Color skin) {
    const int r = sk.armRadius;
    for (int i = 0; i < 2; ++i) {
        Point s = sk.shoulder[i], e = sk.elbow[i], h = sk.hand[i];
        if (sleeves) {
            drawCapsule(canvas, s, e, r * 13 / 10 + 1, Palette::Jacket);
            drawCapsule(canvas, e, h, r * 13 / 10 + 1, Palette::Jacket);
            fillCircle(canvas, h.x, h.y, r + std::max(1, r / 4), skin);
        } else {
            // Draped: the jacket only covers the tops of the arms
            Point part{s.x + (e.x - s.x) * 2 / 5, s.y + (e.y - s.y) * 2 / 5};
            drawCapsule(canvas, s, part, r * 16 / 10 + 1, Palette::Jacket);
        }
    }
}

void drawJacketHeld(Canvas& canvas, Point hand, float scale, float angle) {
    const Matrix3 m = translate(static_cast<float>(hand.x), static_cast<float>(hand.y)) * rotate(angle) * ::scale(scale, scale);
    scanlineFill(canvas, transformPolygon(m, {{-11, 5}, {11, 5}, {19, -58}, {-19, -58}}), Palette::Jacket);
    scanlineFill(canvas, transformPolygon(m, {{-11, 5}, {11, 5}, {9, -6}, {-9, -6}}), Palette::JacketShade);  // fold
    scanlineFill(canvas, transformPolygon(m, {{8, -8}, {15, -8}, {24, -66}, {17, -68}}), Palette::JacketShade);  // sleeve
}

void drawJacketOnFloor(Canvas& canvas, int x, int y, float scale) {
    auto s = [&](float v) { return px(v * scale); };
    fillEllipse(canvas, x, y + s(9), s(42), s(11), Palette::Jacket);
    fillEllipse(canvas, x - s(10), y + s(13), s(18), s(6), Palette::JacketShade);
    scanlineFill(canvas, {{x + s(20), y + s(5)}, {x + s(54), y + s(1)}, {x + s(56), y + s(9)}, {x + s(22), y + s(14)}},
                 Palette::Jacket);
}
