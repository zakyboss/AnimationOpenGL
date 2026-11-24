#include "Heart.h"
#include <algorithm>
#include "../algorithms/Circle.h"
#include "../algorithms/Fill.h"

void drawHeart(Canvas& canvas, int x, int y, int size, Color color) {
    if (size < 4) return;
    const int r = size / 4 + 1;
    fillCircle(canvas, x - size / 4, y + size / 8, r, color);
    fillCircle(canvas, x + size / 4, y + size / 8, r, color);
    scanlineFill(canvas, {{x - size / 2, y + size / 8}, {x + size / 2, y + size / 8}, {x, y - size / 2}}, color);
}
