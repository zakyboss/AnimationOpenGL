#include "Camera.h"
#include <algorithm>
#include <cmath>
#include "../transform/Transform2D.h"

void applyCamera(Canvas& frame, float zoom, PointF focus, int shakeX, int shakeY) {
    if (zoom <= 1.001f && shakeX == 0 && shakeY == 0) return;

    static Canvas source(1, 1);
    source = frame;

    // camera = translate(shake) * scaleAbout(zoom, focus); we need its inverse
    const Matrix3 inverse = scaleAbout(1.0f / zoom, 1.0f / zoom, focus) *
                            translate(static_cast<float>(-shakeX), static_cast<float>(-shakeY));
    const int w = frame.width(), h = frame.height();
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            PointF p = apply(inverse, {x + 0.5f, y + 0.5f});
            int sx = std::clamp(static_cast<int>(std::floor(p.x)), 0, w - 1);
            int sy = std::clamp(static_cast<int>(std::floor(p.y)), 0, h - 1);
            frame.setPixel(x, y, source.getPixel(sx, sy));
        }
    }
}
