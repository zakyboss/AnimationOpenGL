#include "Animation.h"
#include <algorithm>
#include <cmath>

namespace {

constexpr float kPi = 3.14159265f;

struct Gait {
    float cyclesPerSecond;
    float hipSwing, kneeBend;    // degrees, for moving across the screen
    float armSwing, elbowBend;   // degrees
    float lift;                  // 0 to 1, knee lift for stepping towards or away from us
};

// lateral is how much of the movement is across the screen (1) rather than
// towards or away from the viewer (0). Across the screen the legs swing out
// sideways like a side view; towards or away the knees lift in turn, which is
// how a step looks from the front or from behind.
Pose gaitPose(float time, int direction, float pace, float lateral, const Gait& g) {
    const float phase = time * g.cyclesPerSecond * pace * 2 * kPi;
    const float facing = 1.0f - lateral;
    Pose p;  // starts from the rest pose, with arms and legs slightly apart
    for (int i = 0; i < 2; ++i) {
        // The two sides are half a cycle apart
        float s = std::sin(phase + i * kPi);
        // The leg swings forward while cos > 0 and pushes back (foot planted) while cos < 0
        float swing = std::cos(phase + i * kPi - 0.5f);
        p.hip[i] += direction * g.hipSwing * s * lateral;
        // The planted leg stays nearly straight; the knee bends as the foot
        // leaves the ground behind and swings through, so it is lifted, not dragged
        p.knee[i] = -direction * (g.kneeBend * std::max(0.0f, swing) + 4.0f) * lateral;
        // From the front or behind, the swinging leg lifts its knee towards us
        p.lift[i] = g.lift * std::max(0.0f, swing) * (0.35f + 0.65f * facing);
        // Each arm swings opposite to the leg on the same side
        p.shoulder[i] -= direction * g.armSwing * s * lateral;
        p.shoulder[i] += (i == Left ? 1 : -1) * 0.3f * g.armSwing * s * facing;
        p.elbow[i] = direction * g.elbowBend * lateral + (i == Left ? 1 : -1) * 0.4f * g.elbowBend * facing;
    }
    return p;
}

}  // namespace

float clamp01(float t) { return std::min(1.0f, std::max(0.0f, t)); }

float lerpf(float a, float b, float t) { return a + (b - a) * t; }

float easeInOut(float t) {
    t = clamp01(t);
    return t * t * (3 - 2 * t);
}

float progress(float time, float start, float end) {
    return end > start ? clamp01((time - start) / (end - start)) : 1.0f;
}

Pose lerpPose(const Pose& a, const Pose& b, float t) {
    Pose r;
    for (int i = 0; i < 2; ++i) {
        r.shoulder[i] = lerpf(a.shoulder[i], b.shoulder[i], t);
        r.elbow[i] = lerpf(a.elbow[i], b.elbow[i], t);
        r.hip[i] = lerpf(a.hip[i], b.hip[i], t);
        r.knee[i] = lerpf(a.knee[i], b.knee[i], t);
        r.lift[i] = lerpf(a.lift[i], b.lift[i], t);
    }
    r.sit = lerpf(a.sit, b.sit, t);
    r.facingAway = lerpf(a.facingAway, b.facingAway, t);
    r.coverFace = lerpf(a.coverFace, b.coverFace, t);
    r.gazeX = lerpf(a.gazeX, b.gazeX, t);
    r.gazeY = lerpf(a.gazeY, b.gazeY, t);
    r.mouth = lerpf(a.mouth, b.mouth, t);
    r.blink = lerpf(a.blink, b.blink, t);
    return r;
}

Pose walkCycle(float time, int direction, float pace, float lateral) {
    return gaitPose(time, direction, pace, lateral, {0.9f, 20.0f, 48.0f, 16.0f, 12.0f, 0.55f});
}

Pose runCycle(float time, int direction, float pace, float lateral) {
    return gaitPose(time, direction, pace, lateral, {1.6f, 36.0f, 75.0f, 40.0f, 80.0f, 0.85f});
}
