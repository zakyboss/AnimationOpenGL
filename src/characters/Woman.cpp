#include "Woman.h"
#include <algorithm>
#include "../algorithms/Circle.h"
#include "../algorithms/Ellipse.h"
#include "../algorithms/Fill.h"
#include "Face.h"

namespace {

const Proportions kProportions = [] {
    Proportions p;
    p.legUpper = 62; p.legLower = 60;
    p.torso = 88;
    p.headRadius = 31;
    p.shoulderHalf = 24; p.hipHalf = 13;
    p.armUpper = 48; p.armLower = 44;
    p.armRadius = 5; p.legRadius = 6;
    return p;
}();

const FaceStyle kFace{Palette::SkinMedium, Palette::HairDark, Palette::LipRed, true};

// Long hair hangs behind the head and shoulders, so it is drawn before the body.
void drawHairBack(Canvas& c, int cx, int cy, int r) {
    fillEllipse(c, cx, cy - r * 35 / 100, r * 6 / 5, r * 29 / 20, Palette::HairDark);
}

void drawHeadFront(Canvas& c, int cx, int cy, int r, Expression e, const FaceMotion& motion = {}) {
    // The face is a skin ellipse inside the hair, so the hair frames it
    fillEllipse(c, cx, cy - r * 6 / 100, r * 90 / 100, r * 94 / 100, Palette::SkinMedium);
    // Side-swept fringe
    fillEllipse(c, cx - r * 35 / 100, cy + r * 78 / 100, r * 50 / 100, r * 16 / 100, Palette::HairDark);
    // Earrings
    for (int sign : {-1, 1})
        fillCircle(c, cx + sign * r * 88 / 100, cy - r * 38 / 100, std::max(2, r / 10), Palette::Gold);
    drawFace(c, cx, cy, r, e, kFace, motion);
}

}  // namespace

void drawWomanHead(Canvas& canvas, int cx, int cy, int r, Expression e) {
    drawHairBack(canvas, cx, cy, r);
    drawHeadFront(canvas, cx, cy, r, e);
}

void drawWomanHairOver(Canvas& canvas, const Skeleton& sk) {
    drawHairBack(canvas, sk.headCenter.x, sk.headCenter.y, sk.headRadius);
}

Skeleton drawWoman(Canvas& canvas, int x, int y, Expression e, float scale, const Pose& pose) {
    Skeleton sk = buildSkeleton(x, y, scale, kProportions, pose);
    auto s = [&](float v) { return static_cast<int>(v * scale); };
    const bool fromBehind = pose.facingAway > 0.5f;

    // Seen from the front her hair hangs behind her; from behind it hangs over her back
    if (!fromBehind) drawHairBack(canvas, sk.headCenter.x, sk.headCenter.y, sk.headRadius);
    for (Side side : {Left, Right})
        drawLeg(canvas, sk, side, Palette::SkinMedium, Palette::SkinMedium, Palette::ShoeRed);

    // Dress: bodice plus a flared skirt that ends just above the knees
    drawTorso(canvas, sk, Palette::DressRed);
    int waist = sk.pelvis.y + s(14);
    scanlineFill(canvas, {{x - s(19), waist}, {x + s(19), waist},
                          {x + s(44), sk.pelvis.y - s(50)}, {x - s(44), sk.pelvis.y - s(50)}},
                 Palette::DressRed);
    fillRect(canvas, x - s(19), waist - s(2), s(38), std::max(2, s(5)), Palette::Gold);

    for (Side side : {Left, Right})
        drawArm(canvas, sk, side, Sleeve::Short, Palette::DressRed, Palette::SkinMedium);
    drawNeck(canvas, sk, Palette::SkinMedium);
    if (fromBehind) {
        fillCircle(canvas, sk.headCenter.x, sk.headCenter.y, sk.headRadius, Palette::HairDark);
        drawHairBack(canvas, sk.headCenter.x, sk.headCenter.y, sk.headRadius);
    } else {
        drawHeadFront(canvas, sk.headCenter.x, sk.headCenter.y, sk.headRadius, e,
                      {pose.gazeX, pose.gazeY, pose.mouth, pose.blink});
    }
    return sk;
}
