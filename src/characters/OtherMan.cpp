#include "OtherMan.h"
#include <algorithm>
#include <cmath>
#include "../algorithms/Circle.h"
#include "../algorithms/Fill.h"
#include "../algorithms/Line.h"
#include "Face.h"

namespace {

const Proportions kProportions = [] {
    Proportions p;
    p.legUpper = 64; p.legLower = 60;
    p.torso = 98;
    p.headRadius = 34;
    p.shoulderHalf = 36; p.hipHalf = 18;
    p.armUpper = 52; p.armLower = 48;
    p.armRadius = 7.5f; p.legRadius = 9;
    return p;
}();

const FaceStyle kFace{Palette::SkinLight, Palette::HairBrown, Palette::MouthDark, false};

// Ring of small circles over the top of the head.
void drawCurlyHair(Canvas& c, int cx, int cy, int r) {
    for (int deg = 10; deg <= 170; deg += 20) {
        float a = deg * 3.14159265f / 180.0f;
        int hx = cx + static_cast<int>(std::lround(std::cos(a) * r * 0.9f));
        int hy = cy + static_cast<int>(std::lround(r * 0.38f + std::sin(a) * r * 0.62f));
        fillCircle(c, hx, hy, r * 3 / 10, Palette::HairBrown);
    }
    fillCircle(c, cx, cy + r * 3 / 4, r * 2 / 5, Palette::HairBrown);
}

void drawGlasses(Canvas& c, int cx, int cy, int r) {
    int lens = r * 24 / 100;
    int t = std::max(1, r / 18);
    for (int sign : {-1, 1}) {
        int ex = cx + sign * r * 36 / 100;
        for (int k = 0; k < t; ++k) drawCircle(c, ex, cy, lens + k, Palette::GlassesFrame);
        drawThickLine(c, ex + sign * (lens + t), cy + r / 20, cx + sign * r * 96 / 100, cy + r / 10, t, Palette::GlassesFrame);
    }
    int inner = r * 36 / 100 - lens;
    drawThickLine(c, cx - inner, cy + r / 20, cx + inner, cy + r / 20, t, Palette::GlassesFrame);
}

}  // namespace

Skeleton drawOtherMan(Canvas& canvas, int x, int y, Expression e, float scale, const Pose& pose) {
    Skeleton sk = buildSkeleton(x, y, scale, kProportions, pose);
    const int cx = sk.headCenter.x, cy = sk.headCenter.y, r = sk.headRadius;

    for (Side side : {Left, Right})
        drawLeg(canvas, sk, side, Palette::Khaki, Palette::Khaki, Palette::ShoeBlack);
    drawPelvis(canvas, sk, Palette::Khaki);
    drawTorso(canvas, sk, Palette::ShirtMaroon, static_cast<int>(6 * scale));
    for (Side side : {Left, Right})
        drawArm(canvas, sk, side, Sleeve::Long, Palette::ShirtMaroon, Palette::SkinLight);

    drawNeck(canvas, sk, Palette::SkinLight);
    drawEars(canvas, sk, Palette::SkinLight);
    fillCircle(canvas, cx, cy, r, Palette::SkinLight);
    drawCurlyHair(canvas, cx, cy, r);
    drawFace(canvas, cx, cy, r, e, kFace);
    drawGlasses(canvas, cx, cy, r);
    return sk;
}
