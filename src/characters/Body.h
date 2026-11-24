#pragma once
#include "../core/Canvas.h"
#include "../core/Geometry.h"

// Screen left / screen right (the characters face the viewer).
enum Side { Left = 0, Right = 1 };

// Joint angles in degrees. Shoulder and hip angles are measured from straight
// down; positive values swing the limb towards screen right. Elbow and knee
// angles are relative to the upper segment. The animation blends these values.
struct Pose {
    float shoulder[2] = {-8.0f, 8.0f};
    float elbow[2] = {0.0f, 0.0f};
    float hip[2] = {-3.0f, 3.0f};
    float knee[2] = {0.0f, 0.0f};
    // 0 to 1: the knee raised towards the viewer (a step seen from the front or behind)
    float lift[2] = {0.0f, 0.0f};

    // How the body is held, besides the joint angles
    float sit = 0.0f;         // 0 standing, 1 sitting (the thighs point at the viewer, so they are drawn shortened)
    float facingAway = 0.0f;  // above 0.5 the character is seen from behind
    float coverFace = 0.0f;   // 0 to 1: hands moving up over the face, for crying

    // Face
    float gazeX = 0.0f, gazeY = 0.0f;  // where the eyes look, -1 to 1 (left/down to right/up)
    float mouth = 0.0f;                // above 0.3 the mouth is open (talking)
    float blink = 0.0f;                // above 0.5 the eyes are shut
};

// Body measurements in pixels at scale 1.
struct Proportions {
    float legUpper = 66, legLower = 64;
    float torso = 96;
    float neck = 12;
    float headRadius = 32;
    float shoulderHalf = 30, hipHalf = 14;
    float armUpper = 52, armLower = 48;
    float armRadius = 6, legRadius = 7.5f;
};

// Joint positions for one frame, worked out from the feet point, scale and pose.
// The draw functions return this so props (flowers, phone, gift) can be
// attached to a hand.
struct Skeleton {
    float scale = 1.0f;
    float sit = 0.0f;  // copied from the pose, so the legs can be drawn seated
    Point pelvis, neckBase, headCenter;
    int headRadius = 0, armRadius = 0, legRadius = 0;
    Point shoulder[2], elbow[2], hand[2];
    Point hip[2], knee[2], foot[2];
};

// (x, y) is the point between the feet.
Skeleton buildSkeleton(int x, int y, float scale, const Proportions& p, const Pose& pose);

// A limb segment: a filled quad (scanline fill) with round ends (filled circles).
void drawCapsule(Canvas& canvas, Point a, Point b, int radius, Color color);

enum class Sleeve { None, Short, Long };

void drawArm(Canvas& canvas, const Skeleton& sk, Side side, Sleeve sleeve, Color cloth, Color skin);
void drawLeg(Canvas& canvas, const Skeleton& sk, Side side, Color upper, Color lower, Color shoe);
void drawPelvis(Canvas& canvas, const Skeleton& sk, Color color);
// Trapezoid from the shoulders to the hips; bottomWiden adds width at the waist.
void drawTorso(Canvas& canvas, const Skeleton& sk, Color color, int bottomWiden = 0);
void drawNeck(Canvas& canvas, const Skeleton& sk, Color skin);
// Small ellipses either side of the head, drawn before the head itself.
void drawEars(Canvas& canvas, const Skeleton& sk, Color skin);
