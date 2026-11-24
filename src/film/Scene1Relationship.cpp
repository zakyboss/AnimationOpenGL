#include "Scenes.h"
#include <cmath>
#include "../algorithms/Curve.h"
#include "../algorithms/Ellipse.h"
#include "../animation/Animation.h"
#include "../algorithms/Line.h"
#include "../objects/Flower.h"
#include "../objects/Fountain.h"
#include "../objects/Heart.h"
#include "../objects/Jacket.h"
#include "../objects/StreetLife.h"
#include "../scenes/Scene1Park.h"
#include "Cast.h"
#include "Dialogue.h"
#include "Effects.h"

// Scene 1: The Relationship (park, afternoon).
//
//  0:00  Title over the park. The sun shines, clouds drift, birds fly over,
//        and a little bird bathes in the fountain.
//  0:03  The couple walk in hand in hand, chatting; a woman passes them the
//        other way, an old man strolls along the back.
//  0:09  They stop and laugh together.
//  0:12  A cold gust of wind: she wraps her arms round herself and shivers.
//  0:14  He pulls off his jacket and puts it round her shoulders,
//        then holds her tight from behind. A jogger runs past.
//  0:27  She covers her eyes; he sneaks off to the bench where he hid
//        flowers, and comes back. She opens her eyes: flowers!
//  0:37  Hearts float up. She steps close, he puts his arm round her,
//        and they walk off together, still talking.

namespace {

constexpr float kGround = 60;

// Jacket and flower hand-overs
constexpr float kJacketOff = 15.0f, kJacketOn = 18.5f;
constexpr float kHug = 20.2f, kHugEnd = 25.0f;
constexpr float kFlowersPicked = 30.6f, kFlowersGiven = 36.0f, kFlowersTaken = 36.6f;

const Track kWoman = {
    {{0, -300, kGround},
     {1.5f, -300, kGround},
     {9.5f, 470, kGround, 1, Gait::Walk, SwingLeft},   // her right hand holds his
     {38.6f, 470, kGround},
     {39.0f, 520, kGround},                            // steps in close to him
     {45.0f, 520, kGround},
     {56.0f, 1450, kGround, 1, Gait::Walk, SwingLeft}},  // flowers in her right hand
    {{0, arms(-8, 0, 40, 0)},           // holding his hand
     {12.0f, hugSelf()},                // cold
     {18.4f, arms(-25, 60, 25, -60)},   // pulls the jacket round her
     {19.9f, arms(-20, 75, 20, -75)},   // hands resting on his arms
     {25.3f, rest()},
     {26.8f, coverFace()},              // covers her eyes
     {33.4f, surprised()},              // flowers!
     {34.9f, rest()},
     {35.4f, arms(-8, 0, 55, 0)},       // reaches for them
     {36.8f, arms(-8, 0, 10, -80)}},    // holds them to her chest
    {{0, Expression::Happy}, {12.0f, Expression::Neutral}, {15.5f, Expression::Happy},
     {33.4f, Expression::Shocked}, {34.6f, Expression::Happy}},
    {{0, 1, 0}, {12.0f, 0, -0.5f}, {14.2f, 1, 0.2f}, {19.9f, 0.6f, 0.3f}, {25.5f, 1, 0}, {45.0f, 1, 0.1f}},
    talkSpans(sceneLines(1), Speaker::Woman),
};

const Track kBoyfriend = {
    {{0, -139, kGround},
     {1.5f, -139, kGround},
     {9.5f, 631, kGround, 1, Gait::Walk, SwingRight},  // his left hand holds hers
     {17.0f, 631, kGround},
     {17.8f, 590, kGround},             // steps in to put the jacket round her
     {19.3f, 590, kGround},
     {19.9f, 470, 76},                  // moves right behind her to hold her
     {25.2f, 470, 76},
     {25.9f, 600, kGround},
     {27.6f, 600, kGround},
     {28.6f, 520, 80},                  // sneaks behind her to the bench
     {30.0f, 330, 80},
     {30.9f, 330, 80},
     {32.4f, 520, 80},
     {33.2f, 631, kGround},
     {45.0f, 631, kGround},
     {56.0f, 1561, kGround, 1, Gait::Walk, SwingRight}},  // left arm round her
    {{0, arms(-26, 0, 8, 0)},           // holding her hand
     {12.0f, rest()},
     {14.0f, arms(-150, -30, 150, 30)}, // pulls the jacket off over his shoulders
     {15.2f, arms(-25, 10, 8, 0)},      // jacket hanging from his hand
     {17.4f, arms(-115, 30, 8, 0)},     // lifts it round her shoulders
     {18.6f, rest()},
     {19.9f, arms(-12, 102, 12, -102)}, // arms wrapped round her
     {25.3f, rest()},
     {30.0f, arms(-30, 0, 8, 0)},       // reaches down to the bench
     {30.9f, arms(-20, 60, 8, 0)},      // carries the flowers
     {33.8f, arms(-45, 20, 8, 0)},      // holds them out to her
     {36.6f, rest()},
     {39.2f, arms(-95, -10, 8, 0)}},    // arm round her shoulders
    {{0, Expression::Happy}, {12.0f, Expression::Neutral}, {13.2f, Expression::Happy}},
    {{0, -1, 0}, {12.0f, -1, -0.2f}, {19.9f, -0.6f, -0.4f}, {25.9f, -1, 0}, {30.0f, -0.5f, -0.7f},
     {30.9f, 1, 0}, {32.2f, -1, 0}, {45.0f, -1, 0.1f}},
    talkSpans(sceneLines(1), Speaker::Boyfriend),
};

// People going about their day in the park
const PasserbyLook kOldManLook{Palette::SkinLight, Palette::HairGrey, Palette::ShirtGreen, Palette::TrouserGrey,
                               Palette::ShoeBrown, HairStyle::Bald};
const PasserbyLook kWalkerLook{Palette::SkinDeep, Palette::HairBlack, Palette::TopYellow, Palette::SkirtPurple,
                               Palette::ShoeBlack, HairStyle::Long, true};
const PasserbyLook kJoggerLook{Palette::SkinTan, Palette::CapRed, Palette::ShirtWhite, Palette::TrouserDark,
                               Palette::Sneaker, HairStyle::Cap};
const PasserbyLook kManLook{Palette::SkinMedium, Palette::HairBlack, Palette::ShirtOrange, Palette::Denim,
                            Palette::ShoeBlack, HairStyle::Short};

const Track kOldMan = {{{0, 1400, 215, 0.7f}, {32.0f, -100, 215, 0.7f}}, {}, {{0, Expression::Neutral}}, {{0, -1, 0}}, {}};
const Track kWalker = {
    {{2.0f, 1400, 24, 1.05f}, {14.5f, -160, 24, 1.05f}},
    {},
    {{0, Expression::Happy}},
    {{0, -1, 0}, {7.6f, 0.7f, 0.2f}, {9.6f, -1, 0}},  // glances at the couple as she passes
    {},
};
const Track kJogger = {{{16.0f, -120, 232, 0.62f}, {21.0f, 1420, 232, 0.62f, Gait::Run}}, {}, {{0, Expression::Neutral}}, {{0, 1, 0}}, {}};
const Track kMan = {{{41.0f, 1400, 205, 0.72f}, {60.0f, -100, 205, 0.72f}}, {}, {{0, Expression::Neutral}}, {{0, -1, 0}}, {}};

const Canvas& background() {
    static const Canvas cached = [] {
        Canvas c(1280, 720);
        drawParkBackground(c, 0.0f, false);
        return c;
    }();
    return cached;
}

bool between(float t, float a, float b) { return t >= a && t < b; }

void drawSky(Canvas& canvas, float t) {
    // Rays turning slowly round the sun
    const int sunX = 1140, sunY = 610;
    for (int k = 0; k < 12; ++k) {
        float a = (k * 30.0f + t * 6.0f) * 3.14159f / 180.0f;
        drawThickLine(canvas, sunX + static_cast<int>(66 * std::cos(a)), sunY + static_cast<int>(66 * std::sin(a)),
                      sunX + static_cast<int>(88 * std::cos(a)), sunY + static_cast<int>(88 * std::sin(a)), 4, Palette::Sun);
    }
    // Clouds drift slowly to the right and wrap round
    const int cloudX[] = {220, 620, 1050};
    const int cloudY[] = {600, 640, 585};
    for (int k = 0; k < 3; ++k) {
        int x = static_cast<int>(std::fmod(cloudX[k] + t * 9.0f + 200, 1680.0f)) - 200;
        drawCloud(canvas, x, cloudY[k], 55 + 10 * k);
    }
    // Two small flocks of birds
    for (int k = 0; k < 3; ++k) {
        float flap = 0.5f + 0.5f * std::sin(t * 9 + k * 2);
        if (between(t, 1, 14)) drawBird(canvas, static_cast<int>(-60 + (t - 1) * 110 - k * 45), 560 + k * 18, 11, flap);
        if (between(t, 34, 47)) drawBird(canvas, static_cast<int>(1340 - (t - 34) * 120 + k * 50), 610 - k * 15, 10, flap);
    }
}

// Gusts: streaks of air and leaves blowing across the park
void drawWind(Canvas& canvas, float t) {
    if (!between(t, 12.0f, 15.0f)) return;
    float u = t - 12.0f;
    for (int k = 0; k < 6; ++k) {
        int x = static_cast<int>(-250 + u * 950 - k * 140);
        int y = 180 + k * 55;
        int wave = static_cast<int>(14 * std::sin(t * 6 + k));
        drawQuadBezier(canvas, {x, y}, {x + 70, y + wave}, {x + 150, y}, Palette::Wind, 2, 12);
        int lx = static_cast<int>(-120 + u * 700 - k * 170);
        int ly = 140 + k * 45 + static_cast<int>(30 * std::sin(t * 5 + k * 2));
        fillEllipse(canvas, lx, ly, 6, 3, k % 2 ? Palette::Leaf : Palette::GrassDark);
    }
}

void drawHearts(Canvas& canvas, float t, int midX) {
    const Color colors[] = {Palette::Red, Palette::Pink, Palette::Red};
    for (int k = 0; k < 3; ++k) {
        float start = 37.0f + k * 1.6f;
        float p = progress(t, start, start + 4.0f);
        if (p <= 0.0f || p >= 1.0f) continue;
        int x = midX + static_cast<int>(20 * std::sin(p * 6 + k * 2));
        int y = 390 + static_cast<int>(220 * p);
        float shrink = p < 0.8f ? 1.0f : (1.0f - p) / 0.2f;
        drawHeart(canvas, x, y, static_cast<int>((34 + 8 * k) * shrink), colors[k]);
    }
}

}  // namespace

void playScene1(Canvas& canvas, float t) {
    canvas = background();
    drawSky(canvas, t);
    if (t < kFlowersPicked) drawBouquet(canvas, 292, 150, 0.6f);  // hidden on the bench

    ActorFrame her = actorAt(kWoman, t);
    ActorFrame him = actorAt(kBoyfriend, t);
    if (between(t, 12.2f, 16.5f)) her.x += static_cast<int>(2 * std::sin(t * 55));  // shivering

    const bool laughing = between(t, 7.4f, 8.6f) || between(t, 40.5f, 42.0f);
    const int bounce = laughing ? static_cast<int>(5 * std::fabs(std::sin(t * 10))) : 0;
    const Jacket hisJacket = t < kJacketOff ? Jacket::Worn : Jacket::None;
    const Jacket herJacket = t >= kJacketOn ? Jacket::Draped : Jacket::None;

    Skeleton herSk, hisSk;
    std::vector<Drawable> scene = {
        {150, [&] {
             drawFountain(canvas, 980, 150, t);
             drawBathingBird(canvas, 1046, 172, t);
         }},
        {215, [&] { drawExtra(canvas, kOldManLook, actorAt(kOldMan, t)); }},
        {232, [&] { drawExtra(canvas, kJoggerLook, actorAt(kJogger, t)); }},
        {205, [&] { drawExtra(canvas, kManLook, actorAt(kMan, t)); }},
        {24, [&] { drawExtra(canvas, kWalkerLook, actorAt(kWalker, t)); }},
        {him.y + 1, [&] { hisSk = drawActor(canvas, Character::Boyfriend, him, bounce, hisJacket); }},
        {her.y, [&] { herSk = drawActor(canvas, Character::Woman, her, bounce, herJacket); }},
    };
    drawByDepth(scene);

    // Holding her from behind: his arms come round in front of her
    if (between(t, kHug, kHugEnd)) drawArmsOver(canvas, Character::Boyfriend, hisSk);

    if (between(t, kJacketOff, kJacketOn)) drawJacketHeld(canvas, hisSk.hand[Left], 1.0f);

    // The flowers: in his hand, passed over, then hers
    if (t >= kFlowersPicked) {
        Point from = hisSk.hand[Left], to = herSk.hand[Right];
        float pass = easeInOut(progress(t, kFlowersGiven, kFlowersTaken));
        drawBouquet(canvas, static_cast<int>(lerpf(from.x, to.x, pass)), static_cast<int>(lerpf(from.y, to.y, pass)), 0.75f);
    }

    drawHearts(canvas, t, (her.x + him.x) / 2);
    drawWind(canvas, t);
    drawSpeechBubbles(canvas, sceneLines(1), t,
                      {{Speaker::Woman, herSk.headCenter, herSk.headRadius},
                       {Speaker::Boyfriend, hisSk.headCenter, hisSk.headRadius}});

    float titleIn = progress(t, 0.5f, 1.5f), titleOut = progress(t, 4.5f, 5.5f);
    drawCaption(canvas, 520, "Behind the Smile", 8, titleIn * (1.0f - titleOut));
}
