#include "Window.h"
#include <algorithm>
#include "../algorithms/Fill.h"
#include "../algorithms/Line.h"

void drawWindow(Canvas& canvas, int x, int y, int w, int h, bool curtains, Color glass) {
    int frame = std::max(3, w / 20);
    fillRect(canvas, x - frame, y - frame, w + 2 * frame, h + 2 * frame, Palette::WindowFrame);
    fillRect(canvas, x, y, w, h, glass);

    // Light reflections on the glass
    int shine = std::max(2, w / 30);
    drawThickLine(canvas, x + w / 8, y + h / 3, x + w / 3, y + h * 7 / 8, shine, lerp(glass, Palette::White, 0.45f));
    drawThickLine(canvas, x + w * 5 / 8, y + h / 8, x + w * 3 / 4, y + h * 3 / 8, shine, lerp(glass, Palette::White, 0.45f));

    // Glazing bars and sill
    drawThickLine(canvas, x + w / 2, y, x + w / 2, y + h, frame, Palette::WindowFrame);
    drawThickLine(canvas, x, y + h / 2, x + w, y + h / 2, frame, Palette::WindowFrame);
    fillRect(canvas, x - 2 * frame, y - 2 * frame, w + 4 * frame, frame + 2, lerp(Palette::WindowFrame, Palette::Black, 0.1f));

    if (!curtains) return;
    // Curtains tied back at the sides, with a rail across the top
    int top = y + h + frame;
    int bottom = y - frame;
    int mid = y + h * 2 / 5;
    scanlineFill(canvas, {{x - 2 * frame, top}, {x + w * 3 / 10, top}, {x + w / 10, mid},
                          {x + w / 6, bottom}, {x - 2 * frame, bottom}},
                 Palette::Curtain);
    scanlineFill(canvas, {{x + w + 2 * frame, top}, {x + w * 7 / 10, top}, {x + w * 9 / 10, mid},
                          {x + w * 5 / 6, bottom}, {x + w + 2 * frame, bottom}},
                 Palette::Curtain);
    drawThickLine(canvas, x - 3 * frame, top + frame, x + w + 3 * frame, top + frame, frame, Palette::Metal);
}
