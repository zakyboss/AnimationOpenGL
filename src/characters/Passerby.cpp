#include "Passerby.h"
#include <algorithm>
#include "../algorithms/Circle.h"
#include "../algorithms/Ellipse.h"
#include "../algorithms/Fill.h"
#include "Face.h"

Skeleton drawPasserby(Canvas& canvas, int x, int y, const PasserbyLook& look, Expression e, float scale,
                      const Pose& pose) {
    Skeleton sk = buildSkeleton(x, y, scale, Proportions{}, pose);
    auto s = [&](float v) { return static_cast<int>(v * scale); };
    const int cx = sk.headCenter.x, cy = sk.headCenter.y, r = sk.headRadius;
    const bool fromBehind = pose.facingAway > 0.5f;

    if (look.hairStyle == HairStyle::Long && !fromBehind)
        fillEllipse(canvas, cx, cy - r / 3, r * 6 / 5, r * 7 / 5, look.hair);

    for (Side side : {Left, Right})
        drawLeg(canvas, sk, side, look.skirt ? look.skin : look.bottom, look.skirt ? look.skin : look.bottom, look.shoes);
    if (!look.skirt) drawPelvis(canvas, sk, look.bottom);
    drawTorso(canvas, sk, look.top);
    if (look.skirt)
        scanlineFill(canvas, {{x - s(18), sk.pelvis.y + s(10)}, {x + s(18), sk.pelvis.y + s(10)},
                              {x + s(34), sk.pelvis.y - s(48)}, {x - s(34), sk.pelvis.y - s(48)}},
                     look.bottom);
    for (Side side : {Left, Right}) drawArm(canvas, sk, side, Sleeve::Short, look.top, look.skin);
    drawNeck(canvas, sk, look.skin);
    drawEars(canvas, sk, look.skin);

    if (fromBehind) {
        fillCircle(canvas, cx, cy, r, look.hairStyle == HairStyle::Bald ? look.skin : look.hair);
        if (look.hairStyle == HairStyle::Long) fillEllipse(canvas, cx, cy - r / 3, r * 6 / 5, r * 7 / 5, look.hair);
        if (look.hairStyle == HairStyle::Cap) fillEllipse(canvas, cx, cy + r * 6 / 10, r * 105 / 100, r * 5 / 10, look.hair);
        return sk;
    }

    switch (look.hairStyle) {
        case HairStyle::Short:
            fillCircle(canvas, cx, cy + r / 10, r, look.hair);
            fillEllipse(canvas, cx, cy - r / 20, r * 95 / 100, r * 92 / 100, look.skin);
            break;
        case HairStyle::Long:
            fillEllipse(canvas, cx, cy - r / 20, r * 90 / 100, r * 94 / 100, look.skin);
            break;
        case HairStyle::Bald:
            fillCircle(canvas, cx, cy, r, look.skin);
            fillEllipse(canvas, cx - r * 9 / 10, cy + r / 6, r / 5, r / 3, Palette::HairGrey);
            fillEllipse(canvas, cx + r * 9 / 10, cy + r / 6, r / 5, r / 3, Palette::HairGrey);
            break;
        case HairStyle::Cap:
            fillCircle(canvas, cx, cy, r, look.skin);
            fillEllipse(canvas, cx, cy + r * 6 / 10, r * 105 / 100, r * 5 / 10, look.hair);
            fillEllipse(canvas, cx + r / 3, cy + r * 35 / 100, r * 8 / 10, r / 6, look.hair);  // peak
            break;
    }
    drawFace(canvas, cx, cy, r, e, {look.skin, Palette::HairBlack, Palette::MouthDark, look.skirt},
             {pose.gazeX, pose.gazeY, pose.mouth, pose.blink});
    return sk;
}
