#include "Phone.h"
#include <algorithm>
#include "../algorithms/Circle.h"
#include "../algorithms/Fill.h"

// Rectangle with circular corners; (x, y) is the bottom-left corner.
static void fillRoundedRect(Canvas& canvas, int x, int y, int w, int h, int r, Color color) {
    fillRect(canvas, x + r, y, w - 2 * r, h, color);
    fillRect(canvas, x, y + r, w, h - 2 * r, color);
    fillCircle(canvas, x + r, y + r, r, color);
    fillCircle(canvas, x + w - 1 - r, y + r, r, color);
    fillCircle(canvas, x + r, y + h - 1 - r, r, color);
    fillCircle(canvas, x + w - 1 - r, y + h - 1 - r, r, color);
}

void drawPhone(Canvas& canvas, int x, int y, float scale, bool screenToUs) {
    auto s = [&](float v) { return std::max(1, static_cast<int>(v * scale)); };
    int w = s(26), h = s(48);
    int left = x - w / 2, bottom = y - h / 2;

    if (!screenToUs) {
        // The back: a dark case, a raised camera block in the top corner with
        // two lenses and a flash, and a small round logo in the middle
        const Color caseColor = lerp(Palette::PhoneBody, Palette::Black, 0.35f);
        fillRoundedRect(canvas, left, bottom, w, h, s(5), caseColor);
        fillRoundedRect(canvas, left + s(3), bottom + h - s(17), s(12), s(14), s(3), Palette::PhoneBody);
        fillCircle(canvas, left + s(7), bottom + h - s(7), s(2.5f), Palette::PhoneCamera);
        fillCircle(canvas, left + s(7), bottom + h - s(13), s(2.5f), Palette::PhoneCamera);
        fillCircle(canvas, left + s(12), bottom + h - s(10), s(1.2f), Palette::White);
        fillCircle(canvas, x, y - s(2), s(3), lerp(caseColor, Palette::White, 0.25f));
        return;
    }

    fillRoundedRect(canvas, left, bottom, w, h, s(5), Palette::PhoneBody);
    fillRect(canvas, left + s(2), bottom + s(6), w - 2 * s(2), h - 2 * s(6), Palette::PhoneScreen);
    fillCircle(canvas, x, bottom + h - s(3), s(1.2f), Palette::PhoneCamera);
    fillRect(canvas, x - s(4), bottom + s(2), 2 * s(4), s(1.5f), Palette::PhoneCamera);
}
