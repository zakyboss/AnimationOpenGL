#include "GiftBox.h"
#include <algorithm>
#include "../algorithms/Circle.h"
#include "../algorithms/Ellipse.h"
#include "../algorithms/Fill.h"

void drawGiftBox(Canvas& canvas, int x, int y, int size) {
    int half = size / 2;
    int boxH = size * 3 / 4;
    int lidH = std::max(2, size / 5);
    int lidOver = size / 16;
    int ribbon = std::max(2, size / 7);

    fillRect(canvas, x - half, y, size, boxH, Palette::GiftBox);
    fillRect(canvas, x - half - lidOver, y + boxH, size + 2 * lidOver, lidH, Palette::GiftLid);
    fillRect(canvas, x - ribbon / 2, y, ribbon, boxH + lidH, Palette::Ribbon);

    // Bow: two loops and a knot on top of the lid
    int top = y + boxH + lidH;
    int loopRx = size / 5, loopRy = std::max(2, size / 9);
    fillEllipse(canvas, x - loopRx, top + loopRy / 2, loopRx, loopRy, Palette::Ribbon);
    fillEllipse(canvas, x + loopRx, top + loopRy / 2, loopRx, loopRy, Palette::Ribbon);
    fillEllipse(canvas, x - loopRx, top + loopRy / 2, loopRx / 2, loopRy / 2, Palette::RibbonShade);
    fillEllipse(canvas, x + loopRx, top + loopRy / 2, loopRx / 2, loopRy / 2, Palette::RibbonShade);
    fillCircle(canvas, x, top + loopRy / 3, std::max(2, size / 12), Palette::RibbonShade);
}
