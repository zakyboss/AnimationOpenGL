#include "Cast.h"
#include <algorithm>
#include "../algorithms/Circle.h"
#include "../animation/Animation.h"
#include "../characters/Boyfriend.h"
#include "../characters/OtherMan.h"
#include "../characters/Thief.h"
#include "../characters/Woman.h"
#include "../objects/Jacket.h"

namespace {

Skeleton drawCharacter(Canvas& canvas, Character who, const ActorFrame& f, int dy) {
    switch (who) {
        case Character::Woman:     return drawWoman(canvas, f.x, f.y + dy, f.face, f.scale, f.pose);
        case Character::Boyfriend: return drawBoyfriend(canvas, f.x, f.y + dy, f.face, f.scale, f.pose);
        case Character::OtherMan:  return drawOtherMan(canvas, f.x, f.y + dy, f.face, f.scale, f.pose);
        case Character::Thief:     return drawThief(canvas, f.x, f.y + dy, f.face, f.scale, f.pose);
    }
    return {};
}

}  // namespace

Color skinOf(Character who) {
    switch (who) {
        case Character::Woman:     return Palette::SkinMedium;
        case Character::Boyfriend: return Palette::SkinDeep;
        case Character::OtherMan:  return Palette::SkinLight;
        case Character::Thief:     return Palette::SkinTan;
    }
    return Palette::SkinMedium;
}

void drawArmsOver(Canvas& canvas, Character who, const Skeleton& sk) {
    for (Side side : {Left, Right}) {
        switch (who) {
            case Character::Woman:     drawArm(canvas, sk, side, Sleeve::Short, Palette::DressRed, Palette::SkinMedium); break;
            case Character::Boyfriend: drawArm(canvas, sk, side, Sleeve::Short, Palette::ShirtBlue, Palette::SkinDeep); break;
            case Character::OtherMan:  drawArm(canvas, sk, side, Sleeve::Long, Palette::ShirtMaroon, Palette::SkinLight); break;
            case Character::Thief:     drawArm(canvas, sk, side, Sleeve::Long, Palette::Hoodie, Palette::SkinTan); break;
        }
    }
}

Skeleton drawActor(Canvas& canvas, Character who, const ActorFrame& f, int dy, Jacket jacket) {
    Skeleton sk = drawCharacter(canvas, who, f, dy);
    const bool fromBehind = f.pose.facingAway > 0.5f;

    // The arms are drawn again on top when they must show in front of something
    // drawn after them: a jacket's body, or the face when the hands cover it.
    // The skeleton already has the hands on the face, so this is the same pair of arms.
    const bool handsOnFace = f.pose.coverFace > 0.05f && !fromBehind;
    if (jacket != Jacket::None) drawJacketBody(canvas, sk, fromBehind);
    if (jacket != Jacket::None || handsOnFace) drawArmsOver(canvas, who, sk);
    if (jacket != Jacket::None) {
        drawJacketSleeves(canvas, sk, jacket == Jacket::Worn, skinOf(who));
        if (fromBehind && who == Character::Woman) drawWomanHairOver(canvas, sk);
    }
    return sk;
}

Skeleton drawExtra(Canvas& canvas, const PasserbyLook& look, const ActorFrame& f) {
    return drawPasserby(canvas, f.x, f.y, look, f.face, f.scale, f.pose);
}

Point betweenHands(const Skeleton& sk) {
    return {(sk.hand[Left].x + sk.hand[Right].x) / 2, (sk.hand[Left].y + sk.hand[Right].y) / 2};
}

void drawByDepth(std::vector<Drawable>& items) {
    std::stable_sort(items.begin(), items.end(), [](const Drawable& a, const Drawable& b) { return a.depth > b.depth; });
    for (Drawable& d : items) d.draw();
}
