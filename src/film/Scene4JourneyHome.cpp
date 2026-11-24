#include "Scenes.h"
#include <cmath>
#include "../algorithms/Circle.h"
#include "../animation/Animation.h"
#include "../objects/Phone.h"
#include "../objects/StreetLife.h"
#include "../scenes/Street.h"
#include "Cast.h"

// Scene 4: The Journey Home (street, night).
//
//  0:00  Without the jacket now, she walks home slowly, arms wrapped round
//        herself against the cold. A man in a hoodie leans in a doorway, smoking.
//  0:09  She stops and takes out her phone, staring at it, sniffing.
//        His eyes go to the phone. He drops his cigarette.
//  0:19  She walks on, eyes on the screen. A car passes.
//  0:31  He pushes off the wall and starts to follow her.

namespace {

constexpr float kFront = 130;
constexpr float kPhoneOut = 9.6f, kCigaretteDropped = 18.5f;

const Track kWoman = {
    {{0.5f, -120, kFront},
     {9.0f, 430, kFront, 1, Gait::Walk, SwingNone},
     {19.5f, 430, kFront},
     {27.0f, 780, kFront, 1, Gait::Walk, SwingLeft}},  // phone in her right hand
    {{0, hugSelf()}, {9.4f, phoneInRightHand()}},
    {{0, Expression::Sad}},
    {{0, 0.2f, -0.4f}, {9.6f, 0.1f, -0.8f}, {16.0f, -0.6f, 0.2f}, {17.0f, 0.1f, -0.8f}},
    {},
};

const Track kThief = {
    {{0, 1065, 208, 0.85f}, {31.0f, 1065, 208, 0.85f}, {38.5f, 880, 180, 0.9f}},
    {{0, arms(-8, 0, 20, -100)},
     {5.0f, arms(-8, 0, 35, -150)},   // a drag on the cigarette
     {6.5f, arms(-8, 0, 20, -100)},
     {12.0f, arms(-8, 0, 35, -150)},
     {13.5f, arms(-8, 0, 20, -100)},
     {kCigaretteDropped, rest()},
     {19.5f, armsCrossed()},
     {30.5f, rest()}},
    {{0, Expression::Neutral}, {14.5f, Expression::Angry}},
    {{0, -0.3f, 0.3f}, {4.0f, 0.4f, 0.2f}, {9.8f, -1, -0.2f}, {20.0f, -1, -0.1f}},
    {},
};

const Canvas& background() {
    static const Canvas cached = [] {
        Canvas c(1280, 720);
        drawStreet(c, 0.85f);
        return c;
    }();
    return cached;
}

}  // namespace

void playScene4(Canvas& canvas, float t) {
    canvas = background();

    const ActorFrame thief = actorAt(kThief, t);
    Skeleton thiefSk = drawActor(canvas, Character::Thief, thief);
    if (t < kCigaretteDropped) {
        drawCigarette(canvas, thiefSk.hand[Right].x, thiefSk.hand[Right].y, t);
    } else {
        // Dropped: it falls to the pavement and glows there
        float f = progress(t, kCigaretteDropped, kCigaretteDropped + 0.4f);
        int y = static_cast<int>(lerpf(static_cast<float>(thiefSk.hand[Right].y), 212.0f, f * f));
        fillCircle(canvas, thiefSk.hand[Right].x + 8, y, 2, Palette::Ember);
    }

    // Sniffs: two small shudders
    int sniff = static_cast<int>(3 * (std::exp(-std::pow((t - 13.0f) * 6, 2)) + std::exp(-std::pow((t - 17.5f) * 6, 2))));
    Skeleton sk = drawActor(canvas, Character::Woman, actorAt(kWoman, t), sniff);
    if (t >= kPhoneOut) drawPhone(canvas, sk.hand[Right].x, sk.hand[Right].y + 10, 0.8f, false);

    // A car goes by on the road, nearer to us than the pavement, so in front of her
    if (t > 22.0f && t < 26.0f) drawCar(canvas, static_cast<int>(-500 + (t - 22.0f) * 700), 20, 2.0f, Palette::CarRed, true);
}
