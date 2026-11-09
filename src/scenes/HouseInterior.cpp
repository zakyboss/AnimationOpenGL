#include "HouseInterior.h"
#include "../algorithms/Circle.h"
#include "../algorithms/Ellipse.h"
#include "../algorithms/Fill.h"
#include "../algorithms/Line.h"
#include "../objects/Door.h"
#include "../objects/Window.h"

static void drawSofa(Canvas& c, int x, int y, int w) {
    const int seatH = 70, backH = 120;
    fillRect(c, x + 20, y + seatH - 10, w - 40, backH, Palette::SofaDark);
    // Seat cushions
    for (int i = 0; i < 3; ++i) {
        int cw = (w - 80) / 3;
        fillRect(c, x + 40 + i * cw + 3, y + 20, cw - 6, seatH, Palette::Sofa);
    }
    // Arms
    for (int ax : {x, x + w - 50}) {
        fillRect(c, ax, y, 50, seatH + 30, Palette::Sofa);
        fillEllipse(c, ax + 25, y + seatH + 30, 25, 14, Palette::Sofa);
    }
    // Feet
    fillRect(c, x + 14, y - 16, 12, 16, Palette::Wood);
    fillRect(c, x + w - 26, y - 16, 12, 16, Palette::Wood);
}

static void drawLamp(Canvas& c, int x, int floor) {
    fillEllipse(c, x, floor + 6, 36, 8, Palette::Metal);
    drawThickLine(c, x, floor + 6, x, floor + 300, 6, Palette::Metal);
    scanlineFill(c, {{x - 56, floor + 290}, {x + 56, floor + 290}, {x + 34, floor + 370}, {x - 34, floor + 370}},
                 Palette::LampShade);
}

static void drawPicture(Canvas& c, int x, int y, int w, int h) {
    fillRect(c, x - 10, y - 10, w + 20, h + 20, Palette::Wood);
    fillRect(c, x, y, w, h, Palette::SkyBottom);
    fillCircle(c, x + w * 3 / 4, y + h * 3 / 4, h / 8, Palette::Sun);
    // Hills as polygons so they stay inside the frame
    scanlineFill(c, {{x, y}, {x + w - 1, y}, {x + w - 1, y + h / 4}, {x + w / 2, y + h / 2}, {x, y + h * 2 / 5}},
                 Palette::Grass);
    scanlineFill(c, {{x + w / 3, y}, {x + w - 1, y}, {x + w - 1, y + h * 2 / 5}, {x + w * 3 / 4, y + h / 3}},
                 Palette::GrassDark);
}

void drawHouseInterior(Canvas& canvas) {
    const int w = canvas.width();
    const int h = canvas.height();
    const int floor = 210;

    // Back wall and floorboards
    fillRect(canvas, 0, floor, w, h - floor, Palette::WallInside);
    fillRect(canvas, 0, 0, w, floor, Palette::FloorWood);
    for (int y = 30; y < floor; y += 36) drawLineBresenham(canvas, 0, y, w - 1, y, Palette::FloorLine);
    for (int row = 0; row * 36 < floor; ++row) {
        int offset = (row % 2) * 110;
        for (int x = offset; x < w; x += 220) drawLineBresenham(canvas, x, row * 36 - 6, x, row * 36 + 30, Palette::FloorLine);
    }
    fillRect(canvas, 0, floor, w, 16, Palette::Skirting);

    drawWindow(canvas, 110, 340, 230, 230, true);
    drawDoor(canvas, 1030, floor, 140, 320);
    drawPicture(canvas, 560, 480, 170, 110);
    drawLamp(canvas, 420, floor);
    drawSofa(canvas, 470, floor + 10, 360);

    // Rug in the middle of the floor
    fillEllipse(canvas, w / 2, 100, 380, 62, Palette::Rug);
    fillEllipse(canvas, w / 2, 100, 320, 44, Palette::RugInner);
    fillEllipse(canvas, w / 2, 100, 250, 28, Palette::Rug);
}
