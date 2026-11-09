#include "Door.h"
#include <algorithm>
#include "../algorithms/Circle.h"
#include "../algorithms/Fill.h"

void drawDoor(Canvas& canvas, int x, int y, int w, int h, bool open) {
    int frame = std::max(3, w / 12);
    fillRect(canvas, x - frame, y, w + 2 * frame, h + frame, Palette::DoorFrame);
    int knobR = std::max(2, w / 16);

    if (!open) {
        fillRect(canvas, x, y, w, h, Palette::DoorWood);
        int inset = w / 7;
        int panelW = w - 2 * inset;
        int panelH = (h - 3 * inset) / 2;
        fillRect(canvas, x + inset, y + inset, panelW, panelH, Palette::DoorPanel);
        fillRect(canvas, x + inset, y + 2 * inset + panelH, panelW, panelH, Palette::DoorPanel);
        fillCircle(canvas, x + w - inset / 2 - knobR, y + h / 2, knobR, Palette::Gold);
        return;
    }

    fillRect(canvas, x, y, w, h, Palette::Doorway);
    // The open door seen edge-on: a narrow trapezoid in front of the hinge side
    int swing = w * 2 / 5;
    std::vector<Point> leaf = {{x, y}, {x - swing, y - h / 20}, {x - swing, y + h + h / 20}, {x, y + h}};
    scanlineFill(canvas, leaf, Palette::DoorWood);
    fillCircle(canvas, x - swing + knobR * 2, y + h / 2, knobR, Palette::Gold);
}
