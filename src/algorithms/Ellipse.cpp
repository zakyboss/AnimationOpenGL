#include "Ellipse.h"
#include "Line.h"

// Walks the first quadrant of the ellipse and calls visit(x, y) for each point.
template <typename Visit>
static void midpointEllipse(int rx, int ry, Visit visit) {
    long long rx2 = static_cast<long long>(rx) * rx;
    long long ry2 = static_cast<long long>(ry) * ry;
    long long x = 0;
    long long y = ry;
    long long px = 0;            // 2 * ry^2 * x
    long long py = 2 * rx2 * y;  // 2 * rx^2 * y

    // Region 1: slope magnitude < 1, step in x.
    // p1 = ry^2 - rx^2*ry + rx^2/4, scaled by 4 to stay in integers.
    long long p = 4 * ry2 - 4 * rx2 * ry + rx2;
    while (px < py) {
        visit(static_cast<int>(x), static_cast<int>(y));
        ++x;
        px += 2 * ry2;
        if (p < 0) {
            p += 4 * (px + ry2);
        } else {
            --y;
            py -= 2 * rx2;
            p += 4 * (px - py + ry2);
        }
    }

    // Region 2: slope magnitude >= 1, step in y.
    // p2 = ry^2(x+1/2)^2 + rx^2(y-1)^2 - rx^2*ry^2, scaled by 4.
    p = ry2 * (2 * x + 1) * (2 * x + 1) + 4 * rx2 * (y - 1) * (y - 1) - 4 * rx2 * ry2;
    while (y >= 0) {
        visit(static_cast<int>(x), static_cast<int>(y));
        --y;
        py -= 2 * rx2;
        if (p > 0) {
            p += 4 * (rx2 - py);
        } else {
            ++x;
            px += 2 * ry2;
            p += 4 * (px - py + rx2);
        }
    }
}

void drawEllipse(Canvas& canvas, int cx, int cy, int rx, int ry, Color color) {
    midpointEllipse(rx, ry, [&](int x, int y) {
        canvas.setPixel(cx + x, cy + y, color);
        canvas.setPixel(cx - x, cy + y, color);
        canvas.setPixel(cx + x, cy - y, color);
        canvas.setPixel(cx - x, cy - y, color);
    });
}

void fillEllipse(Canvas& canvas, int cx, int cy, int rx, int ry, Color color) {
    midpointEllipse(rx, ry, [&](int x, int y) {
        drawSpan(canvas, cx - x, cx + x, cy + y, color);
        drawSpan(canvas, cx - x, cx + x, cy - y, color);
    });
}
