#include "Scenes.h"
#include <cmath>
#include "../algorithms/Circle.h"
#include "../algorithms/Ellipse.h"
#include "../animation/Animation.h"
#include "../objects/GiftBox.h"
#include "../objects/Heart.h"
#include "../scenes/HouseExterior.h"
#include "../scenes/HouseInterior.h"
#include "Camera.h"
#include "Cast.h"
#include "Dialogue.h"
#include "Effects.h"

// Scene 2: The Discovery (his house at sunset).
//
//  0:00  Still wearing his jacket, she skips up to his house with a gift,
//        stops to smile at it, then goes up the path (seen from behind,
//        getting smaller). Two shadows sit close together in the lit window.
//  0:15  She knocks. No answer. She tries the handle; it opens.
//  0:21  Inside, the two men are cuddled up on the sofa, laughing.
//  0:29  She bursts in holding the gift up: "Surprise!" Her face falls.
//        The camera pushes in on her; the music stops; the gift drops.
//  0:31  The men leap up. He rushes towards her, pleading. She cries.

namespace {

constexpr float kInside = 21.0f;
constexpr float kFrontDoorOpens = 17.6f;
constexpr float kSheEnters = 29.0f, kSheSees = 30.4f;
constexpr float kDrop = 31.4f, kLand = 32.15f;

// ---- Outside ----

// Walking away from us she carries the gift by its ribbon in her left hand
const Pose kCarryGift = arms(-20, 0, 8, 0);

const Track kWomanOutside = {
    {{0.5f, -150, 30},
     {6.5f, 640, 30, 1, Gait::Walk, SwingNone},
     {8.2f, 640, 30},
     {14.5f, 640, 236, 0.62f, Gait::Walk, SwingNone, 0.6f},  // up the path, away from us
     {17.6f, 640, 236, 0.62f},
     {19.0f, 640, 248, 0.6f, Gait::Walk, SwingNone, 0.5f}},
    {{0, holdInFront()},
     {8.3f, fromBehind(kCarryGift)},                // gift down at her side
     {14.8f, fromBehind(arms(-20, 0, 150, -15))},   // knocks
     {16.6f, fromBehind(kCarryGift)},
     {17.2f, fromBehind(arms(-20, 0, 40, 0))},      // tries the handle
     {18.0f, fromBehind(kCarryGift)}},
    {{0, Expression::Happy}},
    {{0, 0.3f, 0}, {7.0f, 0, -0.7f}, {7.9f, 0.3f, 0.4f}},
    talkSpans(sceneLines(2), Speaker::Woman),
};

// Two shadows close together in the lit window, swaying as they laugh
void drawSilhouettes(Canvas& canvas, float t) {
    int sway = static_cast<int>(3 * std::sin(t * 2.2f));
    for (int k = 0; k < 2; ++k) {
        int x = 472 + k * 40 + (k == 0 ? sway : -sway);
        int bob = static_cast<int>(2 * std::fabs(std::sin(t * 4 + k)));
        fillEllipse(canvas, x, 344, 30, 22, Palette::Silhouette);
        fillCircle(canvas, x, 392 + bob, 17, Palette::Silhouette);
    }
}

void playOutside(Canvas& canvas, float t) {
    static const Canvas closed = [] { Canvas c(1280, 720); drawHouseExterior(c, false, 1.0f); return c; }();
    static const Canvas open = [] { Canvas c(1280, 720); drawHouseExterior(c, true, 1.0f); return c; }();
    canvas = t >= kFrontDoorOpens ? open : closed;
    drawSilhouettes(canvas, t);

    if (t < 19.0f) {
        ActorFrame her = actorAt(kWomanOutside, t);
        if (t > 15.1f && t < 16.4f) her.pose.elbow[Right] += 25 * std::sin(t * 16);  // knock knock
        int skip = t < 6.5f ? static_cast<int>(3 * std::fabs(std::sin(t * 7))) : 0;
        Skeleton sk = drawActor(canvas, Character::Woman, her, skip, Jacket::Draped);
        const int size = static_cast<int>(52 * her.scale);
        if (her.pose.facingAway < 0.5f) {
            Point hands = betweenHands(sk);
            drawGiftBox(canvas, hands.x, hands.y - static_cast<int>(8 * her.scale), size);
        } else {
            // Hanging from her hand by the ribbon, swinging a little as she walks
            Point hand = sk.hand[Left];
            float sway = t < 14.5f ? 6.0f * std::sin(t * 5.6f) : 0.0f;
            drawGiftBox(canvas, hand.x, hand.y - size, size, sway);
        }
        drawSpeechBubbles(canvas, sceneLines(2), t, {{Speaker::Woman, sk.headCenter, sk.headRadius}});
    }
    fadeToColor(canvas, Palette::Black, progress(t, 18.8f, 20.5f));
}

// ---- Inside ----

const Track kBoyfriend = {
    {{0, 600, 200}, {33.2f, 600, 200}, {34.6f, 760, 125}},
    {{0, sitting(arms(-8, 0, 95, 10))},   // arm round the other man's shoulders, behind him
     {31.6f, rest()},                     // jumps up
     {33.4f, pleading()},
     {40.0f, arms(-8, 0, 60, 0)}},        // reaches out to her
    {{0, Expression::Happy}, {31.2f, Expression::Shocked}, {35.0f, Expression::Sad}},
    {{0, 1, 0}, {31.2f, 1, 0.1f}},
    talkSpans(sceneLines(2), Speaker::Boyfriend),
};

const Track kOtherMan = {
    {{0, 690, 200}},
    {{0, sitting(arms(-25, 40, 8, 0))},   // hand on the boyfriend's knee
     {31.7f, rest()},
     {35.0f, armsCrossed()}},
    {{0, Expression::Happy}, {31.3f, Expression::Shocked}, {35.0f, Expression::Sad}},
    {{0, -1, 0}, {31.3f, 1, 0.1f}, {35.0f, -0.3f, -0.7f}},
    talkSpans(sceneLines(2), Speaker::OtherMan),
};

const Track kWomanDoorway = {
    {{0, 1100, 214, 0.8f}},
    {{0, holdInFront()},
     {29.4f, arms(-160, 25, 160, -25)},  // holds the gift up: "Surprise!"
     {kDrop, [] { Pose p = coverFace(); p.coverFace = 0.55f; return p; }()},  // hands to her mouth
     {36.5f, coverFace()},                // crying
     {41.5f, rest()}},
    {{0, Expression::Happy}, {kSheSees, Expression::Shocked}, {35.0f, Expression::Sad}, {41.5f, Expression::Angry}},
    {{0, -1, 0}},
    talkSpans(sceneLines(2), Speaker::Woman),
};

void playInside(Canvas& canvas, float t) {
    static const Canvas closed = [] { Canvas c(1280, 720); drawHouseInterior(c, false, true); return c; }();
    static const Canvas open = [] { Canvas c(1280, 720); drawHouseInterior(c, true, true); return c; }();
    canvas = t >= kSheEnters ? open : closed;

    // The men laugh together until she walks in
    int bounce = (t > 22.0f && t < 29.0f) ? static_cast<int>(3 * std::fabs(std::sin(t * 9))) : 0;
    // The boyfriend is drawn first so his arm goes round behind the other man's shoulders
    const Skeleton hisSk = drawActor(canvas, Character::Boyfriend, actorAt(kBoyfriend, t), bounce);
    const Skeleton otherSk = drawActor(canvas, Character::OtherMan, actorAt(kOtherMan, t), bounce);
    Skeleton herSk;

    float p = progress(t, 23.5f, 28.5f);
    if (p > 0.0f && p < 1.0f) {
        float shrink = p < 0.8f ? 1.0f : (1.0f - p) / 0.2f;
        drawHeart(canvas, 645 + static_cast<int>(15 * std::sin(p * 6)), 470 + static_cast<int>(160 * p),
                  static_cast<int>(34 * shrink), Palette::Red);
    }

    if (t >= kSheEnters) {
        const ActorFrame her = actorAt(kWomanDoorway, t);
        Skeleton sk = drawActor(canvas, Character::Woman, her, 0, Jacket::Draped);
        herSk = sk;
        const int size = static_cast<int>(52 * her.scale);
        if (t < kDrop) {
            Point hands = betweenHands(sk);
            drawGiftBox(canvas, hands.x, hands.y - static_cast<int>(10 * her.scale), size);
        } else {
            // Falls from above her head to the floor, tumbling as it goes.
            // Where her hands were is found once by drawing that frame off-screen.
            static const Point dropFrom = [] {
                Canvas scratch(1280, 720);
                return betweenHands(drawActor(scratch, Character::Woman, actorAt(kWomanDoorway, kDrop - 0.01f)));
            }();
            // It ends up lying on its side by her feet, where Scene 3 finds it
            float f = progress(t, kDrop, kLand);
            int x = static_cast<int>(lerpf(static_cast<float>(dropFrom.x), static_cast<float>(kGiftRestX), f));
            int y = static_cast<int>(lerpf(static_cast<float>(dropFrom.y), static_cast<float>(kGiftRestY), f * f));
            drawGiftBox(canvas, x, y, kGiftSize, (360.0f - kGiftRestAngle) * -f);  // one and a quarter turns, ending on its side
        }
    }

    // Push in on her face as she sees them, hold, then ease back out
    float zoom = 1.0f + 0.35f * easeInOut(progress(t, kSheSees, kSheSees + 2.5f)) * (1.0f - easeInOut(progress(t, 34.0f, 35.5f)));
    applyCamera(canvas, zoom, {1100.0f, 440.0f});

    std::vector<SpeakerAt> speakers = {{Speaker::Boyfriend, hisSk.headCenter, hisSk.headRadius},
                                       {Speaker::OtherMan, otherSk.headCenter, otherSk.headRadius}};
    if (t >= kSheEnters) speakers.push_back({Speaker::Woman, herSk.headCenter, herSk.headRadius});
    drawSpeechBubbles(canvas, sceneLines(2), t, speakers, {zoom, {1100.0f, 440.0f}});

    fadeToColor(canvas, Palette::Black, 1.0f - progress(t, kInside, kInside + 0.8f));
}

}  // namespace

void playScene2(Canvas& canvas, float t) {
    if (t < kInside) playOutside(canvas, t);
    else playInside(canvas, t);
}
