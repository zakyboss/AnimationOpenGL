#pragma once
#include "../characters/Body.h"

// Small helpers for turning time (in seconds) into motion.

float clamp01(float t);
float lerpf(float a, float b, float t);
// Smooth start and stop (smoothstep), for movements that shouldn't jerk.
float easeInOut(float t);
// How far time is through [start, end], as 0 to 1 (clamped).
float progress(float time, float start, float end);

// Blends every joint angle between two poses.
Pose lerpPose(const Pose& a, const Pose& b, float t);

// Walk and run cycles. direction is +1 when moving towards screen right and
// -1 towards screen left. The returned pose swings the legs and arms in
// opposite phase; buildSkeleton keeps the feet on the ground. pace speeds
// the cycle up or down (0.6 is a slow, sad walk).
Pose walkCycle(float time, int direction, float pace = 1.0f, float lateral = 1.0f);
Pose runCycle(float time, int direction, float pace = 1.0f, float lateral = 1.0f);

// How far a character moves per second (at scale 1 and pace 1) to match each cycle's stride.
constexpr float kWalkSpeed = 150.0f;
constexpr float kRunSpeed = 420.0f;

// A pose with the given arm angles and the legs at rest.
inline Pose arms(float leftShoulder, float leftElbow, float rightShoulder, float rightElbow) {
    Pose p;
    p.shoulder[Left] = leftShoulder;
    p.elbow[Left] = leftElbow;
    p.shoulder[Right] = rightShoulder;
    p.elbow[Right] = rightElbow;
    return p;
}

// Poses shared by the scenes.
inline Pose rest() { return Pose{}; }
inline Pose holdInFront() { return arms(-12, 95, 12, -95); }   // both hands together: gift, clasped hands
inline Pose surprised() { return arms(-120, -50, 120, 50); }   // hands thrown up beside the head
inline Pose armsCrossed() { return arms(-18, 115, 18, -115); }
inline Pose pleading() { return arms(-40, -25, 40, 25); }      // open arms, palms out
inline Pose pointLeft() { return arms(-88, 0, 8, 0); }
inline Pose phoneInRightHand() { return arms(-8, 0, 12, -105); }
inline Pose hugSelf() { return arms(-25, 125, 25, -125); }      // arms wrapped round herself, cold

// Hands over the face, crying. The arms are raised so the hands are near
// the cheeks; the cast drawing finishes the movement onto the face.
inline Pose coverFace() {
    Pose p = arms(-100, 235, 100, -235);
    p.coverFace = 1.0f;
    return p;
}

// Modifiers: the same pose sitting down or seen from behind.
inline Pose sitting(Pose p) {
    p.sit = 1.0f;
    p.hip[Left] = -9.0f;  // knees a little apart
    p.hip[Right] = 9.0f;
    return p;
}
inline Pose fromBehind(Pose p) {
    p.facingAway = 1.0f;
    return p;
}
