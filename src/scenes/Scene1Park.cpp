#include "Scene1Park.h"
#include "../algorithms/Circle.h"
#include "../algorithms/Ellipse.h"
#include "../algorithms/Fill.h"
#include "../algorithms/Line.h"
#include "../objects/Flower.h"

namespace {

float gDarkness = 0.0f;

// Colours sink towards the night sky as it gets dark.
Color dim(Color c) { return lerp(c, Palette::NightTop, gDarkness * 0.62f); }

void drawTree(Canvas& c, int x, int groundY, int height) {
    int trunkW = height / 8;
    fillRect(c, x - trunkW / 2, groundY, trunkW, height / 2, dim(Palette::Bark));
    int r = height / 4;
    int top = groundY + height / 2 + r / 2;
    fillCircle(c, x - r * 2 / 3, top, r, dim(Palette::Leaves));
    fillCircle(c, x + r * 2 / 3, top, r, dim(Palette::Leaves));
    fillCircle(c, x, top + r * 2 / 3, r, dim(Palette::Leaves));
}

void drawBench(Canvas& c, int x, int y) {
    // Seat, backrest and legs
    fillRect(c, x, y + 30, 160, 10, dim(Palette::Wood));
    fillRect(c, x, y + 55, 160, 8, dim(Palette::Wood));
    fillRect(c, x, y + 72, 160, 8, dim(Palette::Wood));
    drawThickLine(c, x + 10, y, x + 10, y + 80, 6, dim(Palette::Bark));
    drawThickLine(c, x + 150, y, x + 150, y + 80, 6, dim(Palette::Bark));
}

void drawLamp(Canvas& c, int x, int ground) {
    drawThickLine(c, x, ground, x, ground + 230, 7, Palette::LampPost);
    fillEllipse(c, x, ground + 238, 16, 12, Palette::LampPost);
    fillEllipse(c, x, ground + 232, 10, 6, lerp(Palette::Kerb, Palette::LampLight, gDarkness));
}

}  // namespace

void drawCloud(Canvas& c, int x, int y, int size, Color color) {
    fillEllipse(c, x, y, size, size / 2, color);
    fillEllipse(c, x - size * 3 / 4, y - size / 6, size * 2 / 3, size / 3, color);
    fillEllipse(c, x + size * 3 / 4, y - size / 6, size * 2 / 3, size / 3, color);
}

void drawParkBackground(Canvas& canvas, float darkness, bool clouds) {
    gDarkness = darkness;
    const int w = canvas.width();
    const int h = canvas.height();
    const int horizon = h * 2 / 5;

    // Sky with the sun by day, the moon and stars by night
    fillVerticalGradient(canvas, horizon, h - 1, lerp(Palette::SkyBottom, Palette::NightBottom, darkness),
                         lerp(Palette::SkyTop, Palette::NightTop, darkness));
    if (darkness < 0.5f) {
        fillCircle(canvas, w - 140, h - 110, 55, Palette::Sun);
    } else {
        for (int i = 0; i < 40; ++i) {
            int sx = (i * 397 + 53) % w;
            int sy = horizon + 40 + (i * 211 + 17) % (h - horizon - 50);
            fillCircle(canvas, sx, sy, i % 5 == 0 ? 2 : 1, Palette::Star);
        }
        fillCircle(canvas, w - 160, h - 120, 40, Palette::Moon);
        fillCircle(canvas, w - 146, h - 110, 34, lerp(Palette::SkyTop, Palette::NightTop, darkness));  // crescent
    }
    if (clouds) {
        drawCloud(canvas, 220, h - 120, 70, dim(Palette::Cloud));
        drawCloud(canvas, 620, h - 80, 55, dim(Palette::Cloud));
    }

    // Rolling grass: a polygon whose top edge is a wave, filled with the scanline algorithm
    std::vector<Point> hill;
    hill.push_back({0, 0});
    for (int x = 0; x <= w; x += 40) {
        int bump = (x / 40) % 2 == 0 ? 0 : 18;
        hill.push_back({x, horizon + bump});
    }
    hill.push_back({w, 0});
    scanlineFill(canvas, hill, dim(Palette::Grass));

    // Path winding towards the horizon
    scanlineFill(canvas, {{w / 2 - 260, 0}, {w / 2 + 260, 0}, {w / 2 + 40, horizon}, {w / 2 - 40, horizon}},
                 dim(Palette::Path));

    // At night the lamp lights a circle of grass around the bench
    if (darkness > 0.5f) fillEllipse(canvas, 300, 112, 170, 40, lerp(dim(Palette::Grass), Palette::LampLight, 0.3f));

    drawTree(canvas, 140, horizon - 20, 260);
    drawTree(canvas, w - 170, horizon - 40, 300);
    drawBench(canvas, 200, horizon - 170);
    drawLamp(canvas, 400, horizon - 175);

    Color petals[] = {Palette::Red, Palette::Pink, Palette::Purple};
    for (int i = 0; i < 9; ++i) drawFlower(canvas, w - 420 + i * 35, 60 + (i % 3) * 25, dim(petals[i % 3]));
}
