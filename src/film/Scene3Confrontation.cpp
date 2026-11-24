#include "Scenes.h"
#include <cmath>
#include "../animation/Animation.h"
#include "../objects/GiftBox.h"
#include "../objects/Jacket.h"
#include "../scenes/HouseInterior.h"
#include "Camera.h"
#include "Cast.h"
#include "Dialogue.h"

// Scene 3: The Confrontation (inside, evening).
//
//  0:00  She storms in and jabs her finger at him; they argue back and forth.
//  0:12  She pulls his jacket off her shoulders and throws it at his feet.
//  0:15  He reaches for her; she steps back. She points at the other man.
//  0:22  She breaks down, face in her hands. He keeps pleading.
//  0:30  One last angry word, then she turns and walks out, stamping the
//        gift flat in the doorway on her way.
//  0:38  The door slams (the camera shakes). He stares at it, hands on his head.
//  0:41  The other man comes to comfort him. He shoves him away,
//        turns his back, and walks to the window.

namespace {

constexpr float kThrow = 12.8f, kJacketLands = 13.6f;
constexpr float kStamp = 36.6f, kSquashed = 36.8f;
constexpr float kSheLeaves = 37.7f, kDoorSlams = 38.2f;

// Seen from behind with one knee raised, about to stamp
const Pose kStampUp = [] {
    Pose p = fromBehind(rest());
    p.lift[Right] = 1.0f;
    return p;
}();

const Track kWoman = {
    {{0, 1100, 214, 0.8f},
     {1.0f, 1100, 214, 0.8f},
     {3.5f, 900, 95, 1.0f},           // storms in
     {15.5f, 900, 95},
     {16.2f, 960, 95},                // steps back from him
     {28.0f, 960, 95},
     {28.5f, 1000, 95},               // and again
     {33.0f, 1000, 95},
     {36.0f, 1075, 204, 0.82f},       // walks to the door, to the gift
     {36.9f, 1075, 204, 0.82f},
     {37.5f, 1100, 214, 0.8f}},       // and out
    {{0, rest()},
     {3.6f, pointLeft()},
     {11.6f, rest()},
     {12.0f, holdInFront()},          // grabs the jacket
     {kThrow, arms(-95, 0, 8, 0)},    // throws it at him
     {13.8f, rest()},
     {17.2f, pointLeft()},            // points at the other man
     {21.5f, rest()},
     {22.0f, coverFace()},            // breaks down
     {30.5f, rest()},
     {33.0f, fromBehind(rest())},     // turns and leaves
     {36.05f, kStampUp},              // lifts her foot over the gift...
     {36.45f, fromBehind(rest())}},   // ...and stamps down on it
    {{0, Expression::Angry}, {22.0f, Expression::Sad}, {30.5f, Expression::Angry}, {33.0f, Expression::Sad}},
    {{0, -1, 0}, {17.2f, -1, 0.2f}, {30.5f, -1, 0}},
    talkSpans(sceneLines(3), Speaker::Woman),
};

const Track kBoyfriend = {
    {{0, 760, 125},
     {15.0f, 760, 125},
     {16.0f, 820, 120},               // steps towards her
     {27.0f, 820, 120},
     {27.6f, 860, 118},
     {33.6f, 860, 118},
     {35.6f, 940, 118, 1, Gait::Walk, SwingNone},  // follows her to the door
     {47.5f, 940, 118},
     {52.5f, 280, 190}},              // walks away to the window, back to us
    {{0, pleading()},
     {13.2f, rest()},                         // the jacket hits the floor
     {15.0f, arms(-8, 0, 70, 0)},             // reaches for her
     {16.6f, pleading()},
     {27.0f, arms(-8, 0, 70, 0)},
     {28.8f, pleading()},
     {33.6f, arms(-8, 0, 80, 0)},             // reaches after her
     {38.4f, fromBehind(rest())},             // stares at the door
     {39.6f, fromBehind(arms(-150, -40, 150, 40))},  // hands on his head
     {42.0f, fromBehind(rest())},
     {45.4f, rest()},                         // turns round
     {45.6f, arms(-80, 0, -55, 0)},           // shoves him away
     {46.4f, rest()},
     {47.0f, fromBehind(rest())}},            // turns his back on him
    {{0, Expression::Sad}, {13.0f, Expression::Shocked}, {14.5f, Expression::Sad}, {45.4f, Expression::Angry},
     {47.0f, Expression::Sad}},
    {{0, 1, 0}, {13.0f, 0.5f, -0.8f}, {14.5f, 1, 0}, {45.4f, -1, 0}},
    talkSpans(sceneLines(3), Speaker::Boyfriend),
};

const Track kOtherMan = {
    {{0, 690, 200},
     {41.0f, 690, 200},
     {43.8f, 840, 130},               // comes over to comfort him
     {45.8f, 840, 130},
     {46.3f, 700, 150, 1, Gait::Walk, SwingNone, 1.6f}},  // stumbles back
    {{0, armsCrossed()},
     {41.0f, rest()},
     {44.0f, arms(-8, 0, 45, 70)},    // hand on his shoulder
     {45.8f, arms(-60, 0, 60, 0)},    // pushed
     {46.8f, rest()},
     {48.0f, hugSelf()}},
    {{0, Expression::Sad}, {45.8f, Expression::Shocked}, {47.5f, Expression::Sad}},
    {{0, 0.4f, -0.6f}, {41.0f, 1, 0}, {48.0f, -1, 0.3f}},
    talkSpans(sceneLines(3), Speaker::OtherMan),
};

bool between(float t, float a, float b) { return t >= a && t < b; }

}  // namespace

void playScene3(Canvas& canvas, float t) {
    static const Canvas open = [] { Canvas c(1280, 720); drawHouseInterior(c, true, true); return c; }();
    static const Canvas closed = [] { Canvas c(1280, 720); drawHouseInterior(c, false, true); return c; }();
    canvas = t < kDoorSlams ? open : closed;

    ActorFrame her = actorAt(kWoman, t);
    ActorFrame him = actorAt(kBoyfriend, t);
    const ActorFrame other = actorAt(kOtherMan, t);

    // Jabbing finger, shaking pleading hands, sobbing shoulders
    if (between(t, 3.8f, 11.5f) || between(t, 17.4f, 21.4f)) her.pose.shoulder[Left] += 8 * std::sin(t * 7);
    if (between(t, 0.4f, 13.0f) || between(t, 16.8f, 27.0f) || between(t, 28.9f, 33.6f)) {
        him.pose.elbow[Left] -= 6 * std::sin(t * 5);
        him.pose.elbow[Right] += 6 * std::sin(t * 5);
    }
    int sob = between(t, 22.3f, 30.3f) ? static_cast<int>(2 * std::fabs(std::sin(t * 7))) : 0;

    // On the floor: the gift she dropped (flattened once she stamps on it),
    // and his jacket once she has thrown it
    drawGiftBox(canvas, kGiftRestX, kGiftRestY, kGiftSize, kGiftRestAngle, easeInOut(progress(t, kStamp, kSquashed)));
    if (t >= kJacketLands) drawJacketOnFloor(canvas, 770, 104, 1.0f);

    Skeleton herSk, hisSk, otherSk;
    std::vector<Drawable> scene = {
        {other.y, [&] { otherSk = drawActor(canvas, Character::OtherMan, other); }},
        {him.y, [&] { hisSk = drawActor(canvas, Character::Boyfriend, him); }},
    };
    if (t < kSheLeaves)
        scene.push_back({her.y, [&] { herSk = drawActor(canvas, Character::Woman, her, sob, t < kThrow ? Jacket::Draped : Jacket::None); }});
    drawByDepth(scene);

    // The thrown jacket flies in an arc, tumbling, to his feet
    if (between(t, kThrow, kJacketLands)) {
        float p = progress(t, kThrow, kJacketLands);
        Point from = betweenHands(herSk);
        int x = static_cast<int>(lerpf(static_cast<float>(from.x), 780.0f, p));
        int y = static_cast<int>(lerpf(static_cast<float>(from.y), 110.0f, p) + 90 * std::sin(3.14159f * p));
        drawJacketHeld(canvas, {x, y}, 1.0f, 540.0f * p);
    }

    // Move in a little while she cries; jolt at the stamp, shake when the door slams
    float zoom = 1.0f + 0.15f * easeInOut(progress(t, 22.0f, 26.0f)) * (1.0f - easeInOut(progress(t, 30.5f, 32.0f)));
    int shake = 0;
    if (between(t, kSquashed, kSquashed + 0.2f)) shake = static_cast<int>(3 * std::sin(t * 90));
    if (between(t, kDoorSlams, kDoorSlams + 0.4f))
        shake = static_cast<int>(7 * std::sin(t * 90) * (1.0f - progress(t, kDoorSlams, kDoorSlams + 0.4f)));
    applyCamera(canvas, zoom, {980.0f, 320.0f}, shake, shake / 2);

    std::vector<SpeakerAt> speakers = {{Speaker::Boyfriend, hisSk.headCenter, hisSk.headRadius},
                                       {Speaker::OtherMan, otherSk.headCenter, otherSk.headRadius}};
    if (t < kSheLeaves) speakers.push_back({Speaker::Woman, herSk.headCenter, herSk.headRadius});
    drawSpeechBubbles(canvas, sceneLines(3), t, speakers, {zoom, {980.0f, 320.0f}, shake, shake / 2});
}
