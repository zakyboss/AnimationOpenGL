#include "Effects.h"
#include "../text/PixelFont.h"

void fadeToColor(Canvas& canvas, Color color, float amount) {
    if (amount <= 0.0f) return;
    for (int y = 0; y < canvas.height(); ++y)
        for (int x = 0; x < canvas.width(); ++x)
            canvas.setPixel(x, y, lerp(canvas.getPixel(x, y), color, amount));
}

void drawCaption(Canvas& canvas, int cy, const std::string& text, int size, float opacity) {
    if (opacity <= 0.0f) return;
    const int cx = canvas.width() / 2;
    // Sample the background in the middle of the text and blend from it
    Color behind = canvas.getPixel(cx, cy + size * 3);
    drawTextCentred(canvas, cx + size / 2 + 1, cy - size / 2 - 1, text, size, lerp(behind, Palette::Black, opacity * 0.6f));
    drawTextCentred(canvas, cx, cy, text, size, lerp(behind, Palette::White, opacity));
}
