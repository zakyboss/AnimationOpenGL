#include "Scenes.h"
#include <cmath>
#include "../algorithms/Circle.h"
#include "../animation/Animation.h"
#include "../objects/Flower.h"
#include "../objects/Fountain.h"
#include "../scenes/Scene1Park.h"
#include "../text/PixelFont.h"
#include "Camera.h"
#include "Cast.h"
#include "Dialogue.h"
#include "Effects.h"

// Scene 6: The Ending (the same park, now at night).
//
//  0:00  She comes back into the park where the day began, alone and cold.
//  0:11  She walks to the bench under the lamp and sits down.
//  0:17  She cries into her hands. The camera slowly moves in.
//  0:19  An older man walking through the park notices her and slows down.
//  0:27  "Hey... are you okay?" She looks up: "No... not really."
//  0:31  He sits beside her and puts his arm round her; she cries on.
//  0:39  He gives her a flower. She takes it, and for the first time since
//        the park that afternoon, she smiles a little, and looks up at the moon.
//  0:46  The picture fades to black and the closing message fades in.

namespace {

constexpr float kFadeStart = 46.5f, kFadeEnd = 50.0f;
constexpr float kTextStart = 50.5f, kTextEnd = 54.0f;
constexpr float kHeSits = 32.2f;
constexpr float kFlowerOut = 39.3f, kFlowerGiven = 41.8f, kFlowerTaken = 42.3f;

const Track kWoman = {
    {{0.5f, 1420, 60},
     {11.0f, 560, 60, 1, Gait::Walk, SwingNone},
     {14.5f, 250, 83, 1, Gait::Walk, SwingNone},  // to the bench
     {55.0f, 250, 83}},
    {{0, hugSelf()},
     {14.6f, rest()},
     {15.0f, sitting(rest())},
     {16.8f, sitting(coverFace())},             // cries
     {29.0f, sitting(rest())},                  // looks up at him
     {33.8f, sitting(coverFace())},             // cries again, held
     {40.0f, sitting(rest())},
     {41.0f, sitting(arms(-8, 0, 40, 0))},      // reaches for the flower
     {42.3f, sitting(arms(-8, 0, 10, -80))}},   // holds it to her chest
    {{0, Expression::Sad}, {43.5f, Expression::Happy}},
    {{0, -0.6f, -0.4f}, {11.0f, -1, -0.2f}, {15.0f, 0, -0.6f}, {29.0f, 1, 0.2f}, {33.8f, 0, 0},
     {40.5f, 1, -0.2f}, {43.5f, 0.4f, 0.8f}},
    talkSpans(sceneLines(6), Speaker::Woman),
};

const PasserbyLook kStrangerLook{Palette::SkinDeep, Palette::HairGrey, Palette::ShirtGreen, Palette::TrouserGrey,
                                 Palette::ShoeBrown, HairStyle::Short};

const Track kStranger = {
    {{19.0f, 1420, 60},
     {27.0f, 420, 72},                          // walking through the park, slows by the bench
     {31.0f, 420, 72},
     {kHeSits, 330, 78},                        // sits beside her
     {55.0f, 330, 78}},
    {{0, rest()},
     {kHeSits + 0.2f, sitting(rest())},
     {33.6f, sitting(arms(-95, -10, 8, 0))},    // arm round her shoulders
     {kFlowerOut, sitting(arms(-95, -10, -35, -60))},  // holds out a flower
     {kFlowerTaken, sitting(arms(-95, -10, 8, 0))}},
    {{0, Expression::Neutral}, {26.0f, Expression::Concerned}, {40.0f, Expression::Happy}},  // worried for her, not crying
    {{0, -1, 0}, {24.0f, -1, -0.3f}, {33.0f, -1, -0.2f}},
    talkSpans(sceneLines(6), Speaker::Stranger),
};

const Canvas& background() {
    static const Canvas cached = [] {
        Canvas c(1280, 720);
        drawParkBackground(c, 1.0f, false);
        return c;
    }();
    return cached;
}

bool between(float t, float a, float b) { return t >= a && t < b; }

}  // namespace

void playScene6(Canvas& canvas, float t) {
    if (t >= kFadeEnd) {
        canvas.clear(Palette::Black);
        // On a black background, fading the text in is just blending its colour up from black
        Color text = lerp(Palette::Black, Palette::White, easeInOut(progress(t, kTextStart, kTextEnd)));
        drawTextCentred(canvas, canvas.width() / 2, 340, "Sometimes, life changes in a single day.", 4, text);
        return;
    }

    canvas = background();

    // A few stars twinkle, and dark clouds drift past the moon
    for (int i = 0; i < 6; ++i) {
        if (std::sin(t * 2.0f + i * 1.9f) > 0.6f) fillCircle(canvas, 140 + i * 190, 560 + (i * 37) % 120, 2, Palette::Star);
    }
    for (int k = 0; k < 2; ++k) {
        int x = static_cast<int>(std::fmod(300 + k * 700 + t * 6.0f, 1600.0f)) - 200;
        drawCloud(canvas, x, 620 - k * 40, 60, lerp(Palette::NightBottom, Palette::Cloud, 0.25f));
    }
    drawFountain(canvas, 980, 150, t, 1.0f);

    const ActorFrame her = actorAt(kWoman, t);
    const ActorFrame him = actorAt(kStranger, t);
    int dy = 0;
    if (between(t, 17.0f, 29.0f) || between(t, 34.0f, 39.8f)) dy = static_cast<int>(2 * std::fabs(std::sin(t * 6.5f)));  // sobbing

    // While he walks up he is nearer the camera; once seated, he is drawn
    // first so his arm goes round behind her
    Skeleton herSk, hisSk;
    if (t < kHeSits) {
        herSk = drawActor(canvas, Character::Woman, her, dy);
        hisSk = drawExtra(canvas, kStrangerLook, him);
    } else {
        hisSk = drawExtra(canvas, kStrangerLook, him);
        herSk = drawActor(canvas, Character::Woman, her, dy);
    }

    // The flower: from his hand to hers
    if (t >= kFlowerOut) {
        float pass = easeInOut(progress(t, kFlowerGiven, kFlowerTaken));
        Point hand{static_cast<int>(lerpf(hisSk.hand[Right].x, herSk.hand[Right].x, pass)),
                   static_cast<int>(lerpf(hisSk.hand[Right].y, herSk.hand[Right].y, pass))};
        drawFlower(canvas, hand.x, hand.y + 30, Palette::Pink, 1.0f);
    }

    // The camera slowly closes in on the bench
    float zoom = 1.0f + 0.4f * easeInOut(progress(t, 17.0f, 48.0f));
    applyCamera(canvas, zoom, {300.0f, 250.0f});
    drawSpeechBubbles(canvas, sceneLines(6), t,
                      {{Speaker::Woman, herSk.headCenter, herSk.headRadius},
                       {Speaker::Stranger, hisSk.headCenter, hisSk.headRadius}},
                      {zoom, {300.0f, 250.0f}});

    fadeToColor(canvas, Palette::Black, easeInOut(progress(t, kFadeStart, kFadeEnd)));
}
