#pragma once
#include <string>
#include <vector>
#include "../animation/Actor.h"
#include "../core/Canvas.h"
#include "../core/Geometry.h"

// Everything the characters say: when, who, the words, and the recorded
// clip. One table per scene drives three things together, so they always
// match: the voice cues in the soundtrack, the moving mouths, and the
// speech bubbles on screen.

enum class Speaker { Woman, Boyfriend, OtherMan, Stranger };

struct Line {
    float time;         // seconds into the scene
    Speaker who;
    std::string text;   // shown in the bubble
    std::string clip;   // file in Resources/ without ".mp3"; empty if there is no recording
    float length;       // seconds of speech in the clip (measured), for the mouth and bubble
};

// The lines of scene 1 to 6, in scene time.
const std::vector<Line>& sceneLines(int scene);

// When the given character is talking, for their Track (the mouth moves).
std::vector<Span> talkSpans(const std::vector<Line>& lines, Speaker who);

// Where a speaker's head is in the scene, so their bubble can point at it.
struct SpeakerAt {
    Speaker who;
    Point head;   // head centre
    int radius;   // head radius
};

// The camera move applied to the frame, so bubbles (drawn after it, at a
// fixed size) still point at the right head.
struct CameraView {
    float zoom = 1.0f;
    PointF focus{};
    int shakeX = 0, shakeY = 0;
};

// Draws a speech bubble for every line being spoken at time t: a rounded box
// (filled rectangles and midpoint circles) with a tail (scanline-filled
// triangle) pointing at the speaker, and the words wrapped inside.
void drawSpeechBubbles(Canvas& canvas, const std::vector<Line>& lines, float t, const std::vector<SpeakerAt>& speakers,
                       const CameraView& camera = {});
