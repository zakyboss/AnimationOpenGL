#include "Thief.h"
#include <algorithm>
#include "../algorithms/Circle.h"
#include "../algorithms/Ellipse.h"
#include "../algorithms/Fill.h"
#include "../algorithms/Line.h"
#include "Face.h"

namespace {

const Proportions kProportions = [] {
    Proportions p;
    p.legUpper = 70; p.legLower = 66;
    p.torso = 96;
    p.neck = 8;
    p.headRadius = 30;
    p.shoulderHalf = 28; p.hipHalf = 13;
    p.armUpper = 54; p.armLower = 50;
    p.armRadius = 6; p.legRadius = 6.5f;
    return p;
}();

const FaceStyle kFace{Palette::SkinTan, Palette::HairBlack, Palette::MouthDark, false};

}  // namespace

Skeleton drawThief(Canvas& canvas, int x, int y, Expression e, float scale, const Pose& pose) {
    Skeleton sk = buildSkeleton(x, y, scale, kProportions, pose);
    auto s = [&](float v) { return static_cast<int>(v * scale); };
    const int cx = sk.headCenter.x, cy = sk.headCenter.y, r = sk.headRadius;

    const bool fromBehind = pose.facingAway > 0.5f;

    // Back of the hood, behind the head and shoulders
    if (!fromBehind) fillEllipse(canvas, cx, cy - r / 10, r * 13 / 10, r * 135 / 100, Palette::Hoodie);

    for (Side side : {Left, Right})
        drawLeg(canvas, sk, side, Palette::TrouserDark, Palette::TrouserDark, Palette::Sneaker);
    drawPelvis(canvas, sk, Palette::TrouserDark);
    drawTorso(canvas, sk, Palette::Hoodie, s(3));
    // Hoodie hem, and on the front a pocket and drawstrings
    fillRect(canvas, sk.hip[Left].x - sk.legRadius - s(3), sk.pelvis.y, sk.hip[Right].x - sk.hip[Left].x + 2 * sk.legRadius + s(6),
             std::max(2, s(8)), Palette::HoodieShade);
    if (!fromBehind) {
        scanlineFill(canvas, {{x - s(20), sk.pelvis.y + s(10)}, {x + s(20), sk.pelvis.y + s(10)},
                              {x + s(14), sk.pelvis.y + s(36)}, {x - s(14), sk.pelvis.y + s(36)}},
                     Palette::HoodieShade);
        for (int sign : {-1, 1})
            drawThickLine(canvas, x + sign * s(7), sk.neckBase.y - s(2), x + sign * s(8), sk.neckBase.y - s(30),
                          std::max(1, s(2)), Palette::Sneaker);
    }

    for (Side side : {Left, Right})
        drawArm(canvas, sk, side, Sleeve::Long, Palette::Hoodie, Palette::SkinTan);

    if (fromBehind) {
        // From behind only the hood shows
        fillEllipse(canvas, cx, cy - r / 10, r * 12 / 10, r * 125 / 100, Palette::Hoodie);
        return sk;
    }
    // Face showing through the opening of the hood
    fillEllipse(canvas, cx, cy - r * 8 / 100, r * 86 / 100, r * 88 / 100, Palette::SkinTan);
    drawFace(canvas, cx, cy, r, e, kFace, {pose.gazeX, pose.gazeY, pose.mouth, pose.blink});
    return sk;
}
