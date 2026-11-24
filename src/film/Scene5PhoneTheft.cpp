#include "Scenes.h"
#include <cmath>
#include "../animation/Animation.h"
#include "../objects/Phone.h"
#include "../objects/StreetLife.h"
#include "../scenes/Street.h"
#include "Camera.h"
#include "Cast.h"
#include "Dialogue.h"

// Scene 5: The Phone Theft (street, night).
//
//  0:00  He creeps up beside her while she stares at her phone.
//  0:03  He snatches it. "Hey!" She throws her hands up. He bolts off right.
//  0:05  She chases him out of the picture, shouting.
//  0:09  They race across again, him far ahead; she slows, and stops,
//        gasping for breath. He is gone.
//  0:16  She looks around, reaches after him, then breaks down crying.
//        A car drives past as if nothing happened.
//  0:29  She wraps her arms round herself and trudges away.

namespace {

constexpr float kFront = 130;
constexpr float kSnatchStart = 3.3f, kSnatchEnd = 3.6f;

const Track kWoman = {
    {{0, 780, kFront},
     {5.4f, 780, kFront},
     {7.4f, 1450, kFront, 1, Gait::Run},       // chases him off the right
     {10.0f, 1450, kFront},
     {10.0f, -150, kFront},                     // ...and on again from the left
     {13.4f, 560, kFront, 1, Gait::Run},        // running out of breath
     {14.3f, 640, kFront},                      // slows to a stop
     {30.5f, 640, kFront},
     {43.0f, -150, kFront, 1, Gait::Walk, SwingNone}},  // trudges away
    {{0, phoneInRightHand()},
     {3.6f, surprised()},
     {5.2f, rest()},
     {14.3f, arms(-20, 20, 20, -20)},           // out of breath
     {16.2f, arms(-8, 0, 70, 0)},               // reaches after him
     {18.6f, rest()},
     {19.6f, coverFace()},                      // breaks down
     {28.0f, rest()},
     {29.0f, hugSelf()}},
    {{0, Expression::Sad}, {3.6f, Expression::Shocked}, {5.2f, Expression::Angry}, {14.3f, Expression::Shocked},
     {16.5f, Expression::Sad}},
    {{0, 0.1f, -0.8f}, {3.6f, 1, 0}, {14.3f, 1, 0}, {15.2f, -1, 0.1f}, {15.9f, 1, 0.1f}, {16.6f, 1, -0.2f},
     {29.0f, -0.5f, -0.5f}},
    talkSpans(sceneLines(5), Speaker::Woman),
};

const Track kThief = {
    {{0, 880, 180, 0.9f},
     {3.0f, 840, 145, 1.0f, Gait::Walk, SwingBoth, 0.4f},  // creeps up beside her
     {3.8f, 840, 145},
     {5.4f, 1450, 145, 1, Gait::Run},
     {9.0f, 1450, 145},
     {9.0f, -150, 205, 0.85f},
     {12.0f, 1450, 205, 0.85f, Gait::Run}},
    {{0, rest()}, {3.0f, arms(-45, 0, 8, 0)}, {3.9f, rest()}},  // grabs the phone
    {{0, Expression::Angry}},
    {{0, -1, -0.3f}, {4.0f, 1, 0}},
    {},
};

const Canvas& background() {
    static const Canvas cached = [] {
        Canvas c(1280, 720);
        drawStreet(c, 0.9f);
        return c;
    }();
    return cached;
}

bool between(float t, float a, float b) { return t >= a && t < b; }

}  // namespace

void playScene5(Canvas& canvas, float t) {
    canvas = background();

    const ActorFrame her = actorAt(kWoman, t);
    const ActorFrame thief = actorAt(kThief, t);

    int dy = 0;
    if (between(t, 14.3f, 19.0f)) dy = static_cast<int>(4 * (1 - progress(t, 14.3f, 19.0f)) * std::fabs(std::sin(t * 7)));  // gasping
    if (between(t, 19.9f, 27.8f)) dy = static_cast<int>(2 * std::fabs(std::sin(t * 7)));                                  // sobbing

    Skeleton herSk, thiefSk;
    std::vector<Drawable> scene = {
        {thief.y, [&] { thiefSk = drawActor(canvas, Character::Thief, thief); }},
        {her.y, [&] { herSk = drawActor(canvas, Character::Woman, her, dy); }},
    };
    drawByDepth(scene);

    // The phone goes from her right hand to his left
    Point from = herSk.hand[Right], to = thiefSk.hand[Left];
    float pass = easeInOut(progress(t, kSnatchStart, kSnatchEnd));
    drawPhone(canvas, static_cast<int>(lerpf(from.x, to.x, pass)), static_cast<int>(lerpf(from.y + 10, to.y + 10, pass)), 0.8f, false);

    // Life goes on: a car passes in front of her while she cries
    if (between(t, 23.0f, 27.0f)) drawCar(canvas, static_cast<int>(-500 + (t - 23.0f) * 700), 20, 2.0f, Palette::ShirtBlue, true);

    float zoom = 1.0f + 0.12f * easeInOut(progress(t, 20.0f, 24.0f)) * (1.0f - easeInOut(progress(t, 28.0f, 29.5f)));
    applyCamera(canvas, zoom, {640.0f, 330.0f});
    drawSpeechBubbles(canvas, sceneLines(5), t, {{Speaker::Woman, herSk.headCenter, herSk.headRadius}},
                      {zoom, {640.0f, 330.0f}});
}
