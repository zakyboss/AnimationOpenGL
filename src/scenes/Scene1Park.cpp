#include "Scene1Park.h"
#include "../algorithms/Circle.h"
#include "../algorithms/Curve.h"
#include "../algorithms/Ellipse.h"
#include "../algorithms/Fill.h"
#include "../algorithms/Line.h"

static void drawCloud(Canvas& c, int x, int y, int size) {
    fillEllipse(c, x, y, size, size / 2, Palette::Cloud);
    fillEllipse(c, x - size * 3 / 4, y - size / 6, size * 2 / 3, size / 3, Palette::Cloud);
    fillEllipse(c, x + size * 3 / 4, y - size / 6, size * 2 / 3, size / 3, Palette::Cloud);
}

static void drawTree(Canvas& c, int x, int groundY, int height) {
    int trunkW = height / 8;
    fillRect(c, x - trunkW / 2, groundY, trunkW, height / 2, Palette::Bark);
    int r = height / 4;
    int top = groundY + height / 2 + r / 2;
    fillCircle(c, x - r * 2 / 3, top, r, Palette::Leaves);
    fillCircle(c, x + r * 2 / 3, top, r, Palette::Leaves);
    fillCircle(c, x, top + r * 2 / 3, r, Palette::Leaves);
}

static void drawBench(Canvas& c, int x, int y) {
    // Seat, backrest and legs
    fillRect(c, x, y + 30, 160, 10, Palette::Wood);
    fillRect(c, x, y + 55, 160, 8, Palette::Wood);
    fillRect(c, x, y + 72, 160, 8, Palette::Wood);
    drawThickLine(c, x + 10, y, x + 10, y + 80, 6, Palette::Bark);
    drawThickLine(c, x + 150, y, x + 150, y + 80, 6, Palette::Bark);
}

static void drawFlower(Canvas& c, int x, int y, Color petal) {
    drawQuadBezier(c, {x, y - 40}, {x - 12, y - 20}, {x, y}, Palette::GrassDark, 2);
    for (int dx : {-7, 7})
        fillCircle(c, x + dx, y, 6, petal);
    for (int dy : {-7, 7})
        fillCircle(c, x, y + dy, 6, petal);
    fillCircle(c, x, y, 4, Palette::Yellow);
}

void drawParkBackground(Canvas& canvas) {
    const int w = canvas.width();
    const int h = canvas.height();
    const int horizon = h * 2 / 5;

    // Sky, sun and clouds
    fillVerticalGradient(canvas, horizon, h - 1, Palette::SkyBottom, Palette::SkyTop);
    fillCircle(canvas, w - 140, h - 110, 55, Palette::Sun);
    drawCloud(canvas, 220, h - 120, 70);
    drawCloud(canvas, 620, h - 80, 55);

    // Rolling grass: a polygon whose top edge is a wave, filled with the scanline algorithm
    std::vector<Point> hill;
    hill.push_back({0, 0});
    for (int x = 0; x <= w; x += 40) {
        int bump = (x / 40) % 2 == 0 ? 0 : 18;
        hill.push_back({x, horizon + bump});
    }
    hill.push_back({w, 0});
    scanlineFill(canvas, hill, Palette::Grass);

    // Path winding towards the horizon
    scanlineFill(canvas, {{w / 2 - 260, 0}, {w / 2 + 260, 0}, {w / 2 + 40, horizon}, {w / 2 - 40, horizon}},
                 Palette::Path);

    drawTree(canvas, 140, horizon - 20, 260);
    drawTree(canvas, w - 170, horizon - 40, 300);
    drawBench(canvas, 200, horizon - 170);

    Color petals[] = {Palette::Red, Palette::Pink, Palette::Purple};
    for (int i = 0; i < 9; ++i)
        drawFlower(canvas, w - 420 + i * 35, 60 + (i % 3) * 25, petals[i % 3]);
}
