#include "Street.h"
#include "../algorithms/Circle.h"
#include "../algorithms/Curve.h"
#include "../algorithms/Ellipse.h"
#include "../algorithms/Fill.h"
#include "../algorithms/Line.h"
#include "../objects/Door.h"

namespace {

struct Building {
    int x, w, h;
    Color color;
};

constexpr int kRoadTop = 100;
constexpr int kPavementTop = 230;

void drawBuilding(Canvas& c, const Building& b, float darkness, int index) {
    Color wall = lerp(b.color, Palette::NightTop, darkness * 0.5f);
    fillRect(c, b.x, kPavementTop, b.w, b.h, wall);
    fillRect(c, b.x - 6, kPavementTop + b.h, b.w + 12, 14, Palette::Parapet);

    // Grid of windows; more of them are lit as it gets darker
    const int winW = 34, winH = 46, gapX = 24, gapY = 30;
    int cols = (b.w - gapX) / (winW + gapX);
    int rows = (b.h - 110) / (winH + gapY);
    int startX = b.x + (b.w - (cols * (winW + gapX) - gapX)) / 2;
    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < cols; ++col) {
            int wx = startX + col * (winW + gapX);
            int wy = kPavementTop + 110 + row * (winH + gapY);
            int hash = (row * 7 + col * 13 + index * 5) % 10;
            bool lit = hash < 2 + static_cast<int>(darkness * 5);
            fillRect(c, wx, wy, winW, winH, lit ? Palette::WindowLit : Palette::WindowDark);
            drawLineBresenham(c, wx + winW / 2, wy, wx + winW / 2, wy + winH - 1, wall);
        }
    }
}

void drawStreetlight(Canvas& c, int x, float darkness) {
    const int base = kRoadTop + 18;
    if (darkness > 0.3f) {
        // Pool of light on the pavement
        Color pool = lerp(Palette::Pavement, Palette::LampLight, 0.45f * darkness);
        fillEllipse(c, x + 60, base + 40, 120, 34, pool);
    }
    drawThickLine(c, x, base, x, 470, 8, Palette::LampPost);
    drawQuadBezier(c, {x, 466}, {x + 10, 500}, {x + 62, 488}, Palette::LampPost, 6);
    fillEllipse(c, x + 64, 482, 28, 10, Palette::LampPost);
    Color bulb = lerp(Palette::Kerb, Palette::LampLight, 0.4f + 0.6f * darkness);
    fillEllipse(c, x + 64, 474, 18, 6, bulb);
    fillRect(c, x - 12, base - 4, 24, 12, Palette::LampPost);
}

}  // namespace

void drawStreet(Canvas& canvas, float darkness) {
    const int w = canvas.width();
    const int h = canvas.height();

    Color top = lerp(Palette::DuskTop, Palette::NightTop, darkness);
    Color bottom = lerp(Palette::DuskBottom, Palette::NightBottom, darkness);
    fillVerticalGradient(canvas, kPavementTop, h - 1, bottom, top);
    if (darkness > 0.5f) fillCircle(canvas, 860, 640, 30, lerp(Palette::NightBottom, Palette::Cloud, darkness));

    const Building buildings[] = {
        {-20, 260, 380, Palette::BuildingA}, {250, 200, 300, Palette::BuildingB}, {460, 280, 440, Palette::BuildingC},
        {750, 220, 330, Palette::BuildingA}, {980, 320, 400, Palette::BuildingB},
    };
    int index = 0;
    for (const Building& b : buildings) drawBuilding(canvas, b, darkness, index++);
    drawDoor(canvas, 320, kPavementTop, 60, 100);
    drawDoor(canvas, 1100, kPavementTop, 70, 110);

    // Pavement with slab joints, then the kerb
    fillRect(canvas, 0, kRoadTop, w, kPavementTop - kRoadTop, Palette::Pavement);
    drawLineBresenham(canvas, 0, 170, w - 1, 170, Palette::PavementLine);
    for (int x = 0; x < w; x += 90) drawLineDDA(canvas, x, kRoadTop + 14, x + 30, kPavementTop - 1, Palette::PavementLine);
    fillRect(canvas, 0, kRoadTop, w, 14, Palette::Kerb);

    // Road with a dashed centre line
    fillRect(canvas, 0, 0, w, kRoadTop, Palette::Road);
    for (int x = 20; x < w; x += 140) drawThickLine(canvas, x, 48, x + 70, 48, 6, Palette::White);

    for (int x : {120, 640, 1160}) drawStreetlight(canvas, x, darkness);
}
