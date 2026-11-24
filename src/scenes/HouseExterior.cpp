#include "HouseExterior.h"
#include "../algorithms/Circle.h"
#include "../algorithms/Ellipse.h"
#include "../algorithms/Fill.h"
#include "../algorithms/Line.h"
#include "../objects/Door.h"
#include "../objects/Window.h"

namespace {

float gEvening = 0.0f;

// In the evening everything outside darkens towards the dusk sky.
Color dim(Color c) { return lerp(c, Palette::DuskTop, gEvening * 0.35f); }

void drawFence(Canvas& c, int x0, int x1, int y) {
    drawThickLine(c, x0, y + 30, x1, y + 30, 6, dim(Palette::Fence));
    drawThickLine(c, x0, y + 62, x1, y + 62, 6, dim(Palette::Fence));
    for (int x = x0; x <= x1; x += 32) {
        scanlineFill(c, {{x - 6, y}, {x + 6, y}, {x + 6, y + 76}, {x, y + 86}, {x - 6, y + 76}}, dim(Palette::Fence));
    }
}

}  // namespace

void drawHouseExterior(Canvas& canvas, bool doorOpen, float evening) {
    gEvening = evening;
    const int w = canvas.width();
    const int h = canvas.height();
    const int ground = 250;  // where the house stands

    fillVerticalGradient(canvas, ground, h - 1, lerp(Palette::SkyBottom, Palette::DuskBottom, evening),
                         lerp(Palette::SkyTop, Palette::DuskTop, evening));
    fillRect(canvas, 0, 0, w, ground, dim(Palette::Grass));
    fillEllipse(canvas, 180, h - 90, 70, 30, dim(Palette::Cloud));
    fillEllipse(canvas, 1080, h - 130, 60, 26, dim(Palette::Cloud));

    // House: chimney behind the roof, walls, then roof
    const int left = 370, right = 910, wallTop = 520;
    fillRect(canvas, 760, 560, 54, 120, dim(Palette::Brick));
    fillRect(canvas, 752, 680, 70, 14, dim(lerp(Palette::Brick, Palette::Black, 0.2f)));
    fillRect(canvas, left, ground, right - left, wallTop - ground, dim(Palette::WallCream));
    scanlineFill(canvas, {{left - 40, wallTop}, {right + 40, wallTop}, {(left + right) / 2, 690}}, dim(Palette::Roof));
    fillRect(canvas, left - 40, wallTop - 10, right - left + 80, 12, dim(lerp(Palette::Roof, Palette::Black, 0.25f)));

    // In the evening the lights are on inside, so the windows glow
    const Color glass = lerp(Palette::Glass, Palette::WindowLit, evening);
    const int doorW = 104, doorH = 190, doorX = (left + right) / 2 - doorW / 2;
    drawDoor(canvas, doorX, ground, doorW, doorH, doorOpen);
    drawWindow(canvas, left + 50, 320, 140, 120, false, glass);
    drawWindow(canvas, right - 190, 320, 140, 120, false, glass);
    fillCircle(canvas, (left + right) / 2, 600, 26, Palette::WindowFrame);
    fillCircle(canvas, (left + right) / 2, 600, 20, glass);

    // Doorstep and garden path down to the bottom of the frame
    fillRect(canvas, doorX - 20, ground - 14, doorW + 40, 14, dim(Palette::Kerb));
    scanlineFill(canvas, {{doorX - 10, ground - 14}, {doorX + doorW + 10, ground - 14}, {doorX + doorW + 150, 0}, {doorX - 150, 0}},
                 dim(Palette::Path));

    // Bushes along the wall
    for (int x : {left + 40, left + 140, right - 140, right - 40}) {
        fillCircle(canvas, x - 28, ground + 18, 32, dim(Palette::GrassDark));
        fillCircle(canvas, x + 28, ground + 18, 32, dim(Palette::GrassDark));
        fillCircle(canvas, x, ground + 40, 36, dim(Palette::Leaves));
    }

    // Tree on the left
    fillRect(canvas, 120, ground - 30, 36, 220, dim(Palette::Bark));
    fillCircle(canvas, 100, ground + 220, 80, dim(Palette::Leaves));
    fillCircle(canvas, 190, ground + 230, 78, dim(Palette::Leaves));
    fillCircle(canvas, 145, ground + 300, 80, dim(Palette::Leaves));

    // Front fence, with a gap for the path
    drawFence(canvas, 16, doorX - 190, 120);
    drawFence(canvas, doorX + doorW + 190, w - 16, 120);
}
