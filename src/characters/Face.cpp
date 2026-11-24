#include "Face.h"
#include <algorithm>
#include <cmath>
#include "../algorithms/Circle.h"
#include "../algorithms/Curve.h"
#include "../algorithms/Ellipse.h"
#include "../algorithms/Line.h"

namespace {

// Converts face-relative coordinates (fractions of the radius) to pixels.
struct FaceFrame {
    int cx, cy, r;
    Point at(float fx, float fy) const {
        return {cx + static_cast<int>(std::lround(fx * r)), cy + static_cast<int>(std::lround(fy * r))};
    }
    int len(float f) const { return std::max(1, static_cast<int>(std::lround(f * r))); }
};

// side is -1 for the left eye and +1 for the right.
void drawEye(Canvas& c, const FaceFrame& f, float side, Expression e, const FaceStyle& style, const FaceMotion& m) {
    const float ex = side * 0.36f;
    float whiteRx = 0.15f, whiteRy = 0.18f, pupil = 0.085f, pupilDy = 0.0f;
    switch (e) {
        case Expression::Shocked: whiteRx = 0.18f; whiteRy = 0.23f; pupil = 0.06f; break;
        case Expression::Angry:   whiteRy = 0.12f; break;
        case Expression::Sad:     pupilDy = -0.05f; break;
        default: break;
    }

    if (m.blink > 0.5f) {
        // Closed eye: a short downward curve
        drawQuadBezier(c, f.at(ex - 0.14f, 0.0f), f.at(ex, -0.07f), f.at(ex + 0.14f, 0.0f), Palette::EyeDark, f.len(0.045f), 8);
    } else {
        Point centre = f.at(ex, 0.0f);
        fillEllipse(c, centre.x, centre.y, f.len(whiteRx), f.len(whiteRy), Palette::White);
        // The pupils move to where the character is looking, staying inside the white
        float lookX = m.gazeX * (whiteRx - pupil) * 0.9f;
        float lookY = m.gazeY * (whiteRy - pupil) * 0.9f;
        Point p = f.at(ex + lookX, pupilDy + lookY);
        fillCircle(c, p.x, p.y, f.len(pupil), Palette::EyeDark);
        Point shine = f.at(ex + lookX + 0.03f, pupilDy + lookY + 0.03f);
        fillCircle(c, shine.x, shine.y, f.len(0.025f), Palette::White);

        // Looking down (at a phone, at the floor): the upper eyelids lower, so
        // the eyes read as downcast. A skin-coloured ellipse the size of the
        // eye, raised so it covers only the top part, plus a dark lid line.
        const float lid = std::clamp((-m.gazeY - 0.4f) / 0.4f, 0.0f, 1.0f) * 0.5f;
        if (lid > 0.0f) {
            const float edge = whiteRy * (1.0f - 2.0f * lid);  // height of the lid's edge above the eye centre
            Point cover = f.at(ex, edge + whiteRy * 1.05f);
            fillEllipse(c, cover.x, cover.y, f.len(whiteRx * 1.25f), f.len(whiteRy * 1.05f), style.skin);
            drawQuadBezier(c, f.at(ex - whiteRx, edge * 0.6f), f.at(ex, edge - 0.03f), f.at(ex + whiteRx, edge * 0.6f),
                           Palette::EyeDark, f.len(0.035f), 8);
        }
    }

    if (style.lashes) {
        Point a = f.at(ex + side * 0.12f, whiteRy * 0.6f);
        Point b = f.at(ex + side * 0.21f, whiteRy * 0.6f + 0.08f);
        drawThickLine(c, a.x, a.y, b.x, b.y, f.len(0.035f), Palette::EyeDark);
    }
    if (e == Expression::Sad) {
        Point t = f.at(ex + side * 0.05f, -0.3f);
        fillEllipse(c, t.x, t.y, f.len(0.045f), f.len(0.08f), Palette::Tear);
    }
}

void drawBrow(Canvas& c, const FaceFrame& f, float side, Expression e, Color color) {
    // Outer end, control point and inner end, with x measured outwards from the centre.
    float ox, oy, mx, my, ix, iy;
    switch (e) {
        case Expression::Happy:   ox = 0.52f; oy = 0.30f; mx = 0.36f; my = 0.42f; ix = 0.20f; iy = 0.33f; break;
        case Expression::Shocked: ox = 0.52f; oy = 0.42f; mx = 0.36f; my = 0.56f; ix = 0.20f; iy = 0.44f; break;
        case Expression::Angry:   ox = 0.54f; oy = 0.38f; mx = 0.36f; my = 0.29f; ix = 0.16f; iy = 0.17f; break;
        case Expression::Concerned: ox = 0.54f; oy = 0.25f; mx = 0.36f; my = 0.31f; ix = 0.18f; iy = 0.35f; break;
        case Expression::Sad:     ox = 0.54f; oy = 0.22f; mx = 0.36f; my = 0.30f; ix = 0.18f; iy = 0.37f; break;
        default:                  ox = 0.52f; oy = 0.28f; mx = 0.36f; my = 0.34f; ix = 0.20f; iy = 0.29f; break;
    }
    drawQuadBezier(c, f.at(side * ox, oy), f.at(side * mx, my), f.at(side * ix, iy), color, f.len(0.06f), 12);
}

void drawMouth(Canvas& c, const FaceFrame& f, Expression e, const FaceStyle& style, const FaceMotion& m) {
    const int t = f.len(0.055f);

    // Talking: the mouth opens to an oval that grows and shrinks with each
    // syllable (wider when smiling, rounder when shocked), showing the tongue
    // when it opens wide
    if (m.mouth > 0.15f) {
        if (e == Expression::Happy) {
            for (float side : {-1.0f, 1.0f}) {
                Point b = f.at(side * 0.58f, -0.26f);
                fillEllipse(c, b.x, b.y, f.len(0.12f), f.len(0.07f), Palette::Blush);
            }
        }
        Point o = f.at(0.0f, -0.58f);
        float rx = e == Expression::Happy ? 0.24f : e == Expression::Shocked ? 0.15f : 0.2f;
        float ry = (e == Expression::Shocked ? 0.12f : 0.05f) + 0.15f * m.mouth;
        fillEllipse(c, o.x, o.y, f.len(rx), f.len(ry), Palette::MouthDark);
        if (m.mouth > 0.5f) {
            Point tongue = f.at(0.0f, -0.58f - ry * 0.45f);
            fillEllipse(c, tongue.x, tongue.y, f.len(rx * 0.6f), f.len(ry * 0.45f), lerp(Palette::MouthDark, Palette::Red, 0.55f));
        }
        return;
    }

    switch (e) {
        case Expression::Happy: {
            for (float side : {-1.0f, 1.0f}) {
                Point b = f.at(side * 0.58f, -0.26f);
                fillEllipse(c, b.x, b.y, f.len(0.12f), f.len(0.07f), Palette::Blush);
            }
            drawQuadBezier(c, f.at(-0.32f, -0.40f), f.at(0.0f, -0.80f), f.at(0.32f, -0.40f), style.mouth, t);
            break;
        }
        case Expression::Shocked: {
            Point m = f.at(0.0f, -0.55f);
            fillEllipse(c, m.x, m.y, f.len(0.12f), f.len(0.17f), Palette::MouthDark);
            break;
        }
        case Expression::Angry: {
            // Gritted teeth
            Point m = f.at(0.0f, -0.56f);
            fillEllipse(c, m.x, m.y, f.len(0.21f), f.len(0.085f), Palette::MouthDark);
            fillEllipse(c, m.x, m.y, f.len(0.16f), f.len(0.04f), Palette::White);
            break;
        }
        case Expression::Concerned:  // a gentler frown
            drawQuadBezier(c, f.at(-0.2f, -0.6f), f.at(0.0f, -0.5f), f.at(0.2f, -0.6f), style.mouth, t);
            break;
        case Expression::Sad:
            drawQuadBezier(c, f.at(-0.24f, -0.64f), f.at(0.0f, -0.48f), f.at(0.24f, -0.64f), style.mouth, t);
            break;
        default:
            drawQuadBezier(c, f.at(-0.18f, -0.52f), f.at(0.0f, -0.56f), f.at(0.18f, -0.52f), style.mouth, t);
            break;
    }
}

}  // namespace

void drawFace(Canvas& canvas, int cx, int cy, int r, Expression e, const FaceStyle& style, const FaceMotion& motion) {
    FaceFrame f{cx, cy, r};
    for (float side : {-1.0f, 1.0f}) {
        drawEye(canvas, f, side, e, style, motion);
        drawBrow(canvas, f, side, e, style.brow);
    }
    Color noseShade = lerp(style.skin, Palette::Black, 0.3f);
    drawQuadBezier(canvas, f.at(0.02f, -0.08f), f.at(0.11f, -0.22f), f.at(-0.03f, -0.26f), noseShade, f.len(0.035f), 10);
    drawMouth(canvas, f, e, style, motion);
}
