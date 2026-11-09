#include "Canvas.h"
#include <algorithm>
#include <cstdint>
#include <fstream>

Canvas::Canvas(int width, int height, Color background)
    : width_(width), height_(height), pixels_(width * height, background) {}

void Canvas::clear(Color c) {
    std::fill(pixels_.begin(), pixels_.end(), c);
}

void Canvas::setPixel(int x, int y, Color c) {
    if (!inBounds(x, y)) return;
    pixels_[y * width_ + x] = c;
}

Color Canvas::getPixel(int x, int y) const {
    if (!inBounds(x, y)) return Palette::Black;
    return pixels_[y * width_ + x];
}

static void writeLE(std::ofstream& out, uint32_t value, int bytes) {
    for (int i = 0; i < bytes; ++i) out.put(static_cast<char>((value >> (8 * i)) & 0xFF));
}

bool Canvas::saveBMP(const std::string& path) const {
    std::ofstream out(path, std::ios::binary);
    if (!out) return false;

    const int rowSize = (width_ * 3 + 3) & ~3;  // rows are padded to 4 bytes
    const uint32_t dataSize = rowSize * height_;

    // File header
    out.put('B'); out.put('M');
    writeLE(out, 54 + dataSize, 4);
    writeLE(out, 0, 4);
    writeLE(out, 54, 4);
    // Info header
    writeLE(out, 40, 4);
    writeLE(out, width_, 4);
    writeLE(out, height_, 4);  // positive height = bottom-up rows, matching our layout
    writeLE(out, 1, 2);
    writeLE(out, 24, 2);
    writeLE(out, 0, 4);
    writeLE(out, dataSize, 4);
    writeLE(out, 2835, 4);
    writeLE(out, 2835, 4);
    writeLE(out, 0, 4);
    writeLE(out, 0, 4);

    std::vector<char> row(rowSize, 0);
    for (int y = 0; y < height_; ++y) {
        for (int x = 0; x < width_; ++x) {
            const Color& c = pixels_[y * width_ + x];
            row[x * 3 + 0] = static_cast<char>(c.b);  // BMP stores BGR
            row[x * 3 + 1] = static_cast<char>(c.g);
            row[x * 3 + 2] = static_cast<char>(c.r);
        }
        out.write(row.data(), rowSize);
    }
    return static_cast<bool>(out);
}
