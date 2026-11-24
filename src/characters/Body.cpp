#include "Body.h"
#include <algorithm>
#include <cmath>
#include "../algorithms/Circle.h"
#include "../algorithms/Ellipse.h"
#include "../algorithms/Fill.h"
#include "../transform/Transform2D.h"

static int px(float v) { return static_cast<int>(std::lround(v)); }

// End point of a segment of the given length hanging straight down from
// origin, rotated about origin by angleDeg (positive swings towards screen right).
static Point rotateFrom(Point origin, float length, float angleDeg) {
    PointF pivot{static_cast<float>(origin.x), static_cast<float>(origin.y)};
    return toPoint(apply(rotateAbout(angleDeg, pivot), PointF{pivot.x, pivot.y - length}));
}

// Moves every joint down by dy pixels.
static void shiftDown(Skeleton& k, int dy) {
    for (Point* p : {&k.pelvis, &k.neckBase, &k.headCenter}) p->y -= dy;
    for (int i = 0; i < 2; ++i)
        for (Point* p : {&k.shoulder[i], &k.elbow[i], &k.hand[i], &k.hip[i], &k.knee[i], &k.foot[i]}) p->y -= dy;
}

Skeleton buildSkeleton(int x, int y, float s, const Proportions& p, const Pose& pose) {
    Skeleton k;
    k.scale = s;
    k.sit = pose.sit;
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
        // Sitting or lifting the knee points the thigh towards the viewer, so it looks shorter
        const float thigh = p.legUpper * (1.0f - 0.85f * pose.sit) * (1.0f - 0.5f * pose.lift[i]);
        const float shin = p.legLower * (1.0f - 0.12f * pose.lift[i]);
        k.knee[i] = rotateFrom(k.hip[i], thigh * s, pose.hip[i]);
        k.foot[i] = rotateFrom(k.knee[i], shin * s, pose.hip[i] + pose.knee[i]);
    }

    // Covering the face: the hands travel from where the arm angles put them
    // onto the eyes. The elbows stay where they are, so the forearms follow.
    if (pose.coverFace > 0.0f) {
        for (int i = 0; i < 2; ++i) {
            int sign = i == Left ? -1 : 1;
            Point onFace{k.headCenter.x + sign * k.headRadius * 3 / 10, k.headCenter.y};
            k.hand[i] = {k.hand[i].x + px((onFace.x - k.hand[i].x) * pose.coverFace),
                         k.hand[i].y + px((onFace.y - k.hand[i].y) * pose.coverFace)};
        }
    }

    // Swinging or bending the legs lifts the feet. Lower the whole body so the
    // lowest foot stays on the ground; this also gives walking its natural bob.
    int lift = std::min(k.foot[Left].y, k.foot[Right].y) - (y + k.legRadius);
    if (lift > 0) shiftDown(k, lift);
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
    int sign = side == Left ? -1 : 1;
    drawCapsule(canvas, k, f, r, lower);
    drawCapsule(canvas, h, k, r, upper);
    if (sk.sit > 0.5f) {
        // Seated, the thigh points at the viewer: seen end-on it is a wide, round lap
        fillEllipse(canvas, (h.x + k.x) / 2 + sign * r / 3, (h.y + k.y) / 2, r * 17 / 10, r * 15 / 10, upper);
    }
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
