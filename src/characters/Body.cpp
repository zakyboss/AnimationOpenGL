#include "Body.h"
#include <algorithm>
#include <cmath>
#include "../algorithms/Circle.h"
#include "../algorithms/Ellipse.h"
#include "../algorithms/Fill.h"

static int px(float v) { return static_cast<int>(std::lround(v)); }

// End point of a segment of the given length hanging from origin, rotated by
// angleDeg away from straight down.
static Point rotateFrom(Point origin, float length, float angleDeg) {
    float rad = angleDeg * 3.14159265f / 180.0f;
    return {origin.x + px(length * std::sin(rad)), origin.y - px(length * std::cos(rad))};
}

Skeleton buildSkeleton(int x, int y, float s, const Proportions& p, const Pose& pose) {
    Skeleton k;
    k.scale = s;
    k.armRadius = std::max(1, px(p.armRadius * s));
    k.legRadius = std::max(1, px(p.legRadius * s));
    // Raised by the leg radius so the rounded end of the leg rests on y.
    k.pelvis = {x, y + px((p.legUpper + p.legLower) * s) + k.legRadius};
    int shoulderY = k.pelvis.y + px(p.torso * s);
    k.neckBase = {x, shoulderY};
    k.headRadius = px(p.headRadius * s);
    k.headCenter = {x, shoulderY + px(p.neck * s) + k.headRadius};

    for (int i = 0; i < 2; ++i) {
        int sign = i == Left ? -1 : 1;
        k.shoulder[i] = {x + sign * px(p.shoulderHalf * s), shoulderY - k.armRadius};
        k.elbow[i] = rotateFrom(k.shoulder[i], p.armUpper * s, pose.shoulder[i]);
        k.hand[i] = rotateFrom(k.elbow[i], p.armLower * s, pose.shoulder[i] + pose.elbow[i]);

        k.hip[i] = {x + sign * px(p.hipHalf * s), k.pelvis.y};
        k.knee[i] = rotateFrom(k.hip[i], p.legUpper * s, pose.hip[i]);
        k.foot[i] = rotateFrom(k.knee[i], p.legLower * s, pose.hip[i] + pose.knee[i]);
    }
    return k;
}

void drawCapsule(Canvas& canvas, Point a, Point b, int radius, Color color) {
    float dx = static_cast<float>(b.x - a.x);
    float dy = static_cast<float>(b.y - a.y);
    float length = std::sqrt(dx * dx + dy * dy);
    if (length > 0.5f) {
        float nx = -dy / length * radius;
        float ny = dx / length * radius;
        scanlineFill(canvas, {{a.x + px(nx), a.y + px(ny)}, {b.x + px(nx), b.y + px(ny)},
                              {b.x - px(nx), b.y - px(ny)}, {a.x - px(nx), a.y - px(ny)}},
                     color);
    }
    fillCircle(canvas, a.x, a.y, radius, color);
    fillCircle(canvas, b.x, b.y, radius, color);
}

static Point between(Point a, Point b, float t) {
    return {a.x + px((b.x - a.x) * t), a.y + px((b.y - a.y) * t)};
}

void drawArm(Canvas& canvas, const Skeleton& sk, Side side, Sleeve sleeve, Color cloth, Color skin) {
    Point s = sk.shoulder[side], e = sk.elbow[side], h = sk.hand[side];
    int r = sk.armRadius;
    switch (sleeve) {
        case Sleeve::Long:
            drawCapsule(canvas, s, e, r, cloth);
            drawCapsule(canvas, e, h, r, cloth);
            break;
        case Sleeve::Short:
            drawCapsule(canvas, s, e, r, skin);
            drawCapsule(canvas, e, h, r, skin);
            drawCapsule(canvas, s, between(s, e, 0.45f), r + std::max(1, r / 3), cloth);
            break;
        case Sleeve::None:
            drawCapsule(canvas, s, e, r, skin);
            drawCapsule(canvas, e, h, r, skin);
            break;
    }
    fillCircle(canvas, h.x, h.y, r + std::max(1, r / 4), skin);
}

void drawLeg(Canvas& canvas, const Skeleton& sk, Side side, Color upper, Color lower, Color shoe) {
    Point h = sk.hip[side], k = sk.knee[side], f = sk.foot[side];
    int r = sk.legRadius;
    drawCapsule(canvas, k, f, r, lower);
    drawCapsule(canvas, h, k, r, upper);
    int sign = side == Left ? -1 : 1;
    int shoeRy = std::max(2, r * 9 / 10);
    // Shoe bottom sits level with the end of the leg, covering it
    fillEllipse(canvas, f.x + sign * r / 2, f.y - r + shoeRy, r * 2, shoeRy, shoe);
}

void drawPelvis(Canvas& canvas, const Skeleton& sk, Color color) {
    int r = sk.legRadius;
    int left = sk.hip[Left].x - r, right = sk.hip[Right].x + r;
    fillRect(canvas, left, sk.pelvis.y - r * 3 / 2, right - left + 1, r * 5 / 2, color);
}

void drawTorso(Canvas& canvas, const Skeleton& sk, Color color, int bottomWiden) {
    int top = sk.neckBase.y;
    int bottom = sk.pelvis.y;
    int topLeft = sk.shoulder[Left].x - sk.armRadius / 2, topRight = sk.shoulder[Right].x + sk.armRadius / 2;
    int botLeft = sk.hip[Left].x - sk.legRadius - bottomWiden;
    int botRight = sk.hip[Right].x + sk.legRadius + bottomWiden;
    scanlineFill(canvas, {{botLeft, bottom}, {botRight, bottom}, {topRight, top}, {topLeft, top}}, color);
    // Rounded shoulders
    int sr = sk.armRadius * 3 / 2;
    fillCircle(canvas, sk.shoulder[Left].x, sk.shoulder[Left].y, sr, color);
    fillCircle(canvas, sk.shoulder[Right].x, sk.shoulder[Right].y, sr, color);
}

void drawNeck(Canvas& canvas, const Skeleton& sk, Color skin) {
    drawCapsule(canvas, sk.neckBase, sk.headCenter, std::max(2, sk.headRadius * 3 / 10), skin);
}

void drawEars(Canvas& canvas, const Skeleton& sk, Color skin) {
    int r = sk.headRadius;
    for (int sign : {-1, 1})
        fillEllipse(canvas, sk.headCenter.x + sign * r * 49 / 50, sk.headCenter.y - r / 20,
                    std::max(2, r * 4 / 25), std::max(2, r / 4), skin);
}
