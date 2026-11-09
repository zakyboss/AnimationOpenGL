#include "Flower.h"
#include <algorithm>
#include "../algorithms/Circle.h"
#include "../algorithms/Curve.h"
#include "../algorithms/Fill.h"

// Four petals around a yellow centre.
static void drawFlowerHead(Canvas& canvas, int x, int y, Color petal, float scale) {
    auto s = [&](float v) { return static_cast<int>(v * scale); };
    int petalR = std::max(2, s(6));
    for (int dx : {-7, 7})
        fillCircle(canvas, x + s(dx), y, petalR, petal);
    for (int dy : {-7, 7})
        fillCircle(canvas, x, y + s(dy), petalR, petal);
    fillCircle(canvas, x, y, std::max(1, s(4)), Palette::Yellow);
}

void drawFlower(Canvas& canvas, int x, int y, Color petal, float scale) {
    auto s = [&](float v) { return static_cast<int>(v * scale); };
    drawQuadBezier(canvas, {x, y - s(40)}, {x - s(12), y - s(20)}, {x, y}, Palette::GrassDark, std::max(1, s(2)));
    drawFlowerHead(canvas, x, y, petal, scale);
}

void drawBouquet(Canvas& canvas, int x, int y, float scale) {
    auto s = [&](float v) { return static_cast<int>(v * scale); };
    const Color petals[] = {Palette::Red, Palette::Pink, Palette::Purple, Palette::Pink, Palette::Red};
    const int offsets[][2] = {{-22, 72}, {22, 72}, {0, 92}, {-11, 58}, {12, 60}};

    // Stems fan out from the hand to each flower head
    for (int i = 0; i < 5; ++i) {
        int hx = x + s(offsets[i][0]), hy = y + s(offsets[i][1]);
        drawQuadBezier(canvas, {x, y}, {x + s(offsets[i][0] / 3.0f), y + s(30)}, {hx, hy},
                       Palette::GrassDark, std::max(1, s(2)));
    }
    for (int i = 0; i < 5; ++i)
        drawFlowerHead(canvas, x + s(offsets[i][0]), y + s(offsets[i][1]), petals[i], scale * 1.3f);

    // Paper cone wrapped around the stems
    scanlineFill(canvas, {{x, y - s(6)}, {x + s(24), y + s(48)}, {x - s(24), y + s(48)}}, Palette::WrapPaper);
    fillCircle(canvas, x, y + s(16), std::max(2, s(5)), Palette::Ribbon);
}
