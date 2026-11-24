#include "Boyfriend.h"
#include <algorithm>
#include "../algorithms/Circle.h"
#include "../algorithms/Ellipse.h"
#include "../algorithms/Fill.h"
#include "Face.h"

namespace {

const Proportions kProportions = [] {
    Proportions p;
    p.legUpper = 70; p.legLower = 68;
    p.torso = 100;
    p.headRadius = 32;
    p.shoulderHalf = 32; p.hipHalf = 16;
    p.armUpper = 56; p.armLower = 50;
    p.armRadius = 6.5f; p.legRadius = 8;
    return p;
}();

const FaceStyle kFace{Palette::SkinDeep, Palette::HairBlack, Palette::MouthDark, false};

}  // namespace

Skeleton drawBoyfriend(Canvas& canvas, int x, int y, Expression e, float scale, const Pose& pose) {
    Skeleton sk = buildSkeleton(x, y, scale, kProportions, pose);
    auto s = [&](float v) { return static_cast<int>(v * scale); };
    const int cx = sk.headCenter.x, cy = sk.headCenter.y, r = sk.headRadius;

    for (Side side : {Left, Right})
        drawLeg(canvas, sk, side, Palette::Denim, Palette::Denim, Palette::ShoeBrown);
    drawPelvis(canvas, sk, Palette::Denim);
    drawTorso(canvas, sk, Palette::ShirtBlue);
    fillRect(canvas, sk.hip[Left].x - sk.legRadius, sk.pelvis.y - s(3), sk.hip[Right].x - sk.hip[Left].x + 2 * sk.legRadius,
             std::max(2, s(6)), Palette::Belt);
    for (Side side : {Left, Right})
        drawArm(canvas, sk, side, Sleeve::Short, Palette::ShirtBlue, Palette::SkinDeep);

    drawNeck(canvas, sk, Palette::SkinDeep);
    if (pose.facingAway > 0.5f) {
        // From behind: the back of his head is all hair, with his ears showing
        drawEars(canvas, sk, Palette::SkinDeep);
        fillCircle(canvas, cx, cy + r * 4 / 100, r, Palette::HairBlack);
        return sk;
    }

    // Shirt collar
    int ny = sk.neckBase.y;
    scanlineFill(canvas, {{x - s(13), ny + s(2)}, {x, ny - s(12)}, {x - s(4), ny + s(2)}}, Palette::White);
    scanlineFill(canvas, {{x + s(13), ny + s(2)}, {x, ny - s(12)}, {x + s(4), ny + s(2)}}, Palette::White);

    // Short cropped hair: the head is filled with hair, then the face drawn inside it
    fillCircle(canvas, cx, cy + r * 9 / 100, r * 102 / 100, Palette::HairBlack);
    drawEars(canvas, sk, Palette::SkinDeep);
    fillEllipse(canvas, cx, cy - r * 5 / 100, r * 96 / 100, r * 93 / 100, Palette::SkinDeep);
    drawFace(canvas, cx, cy, r, e, kFace, {pose.gazeX, pose.gazeY, pose.mouth, pose.blink});
    return sk;
}
