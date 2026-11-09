#pragma once
#include <string>
#include <vector>
#include "Color.h"

// A simple framebuffer. Every algorithm draws into this using setPixel only,
// so all rasterisation is our own C++ code. In the OpenGL phase the pixel
// buffer can be uploaded as a texture each frame.
//
// Coordinates: (0, 0) is the bottom-left corner, y points up (same as OpenGL).
class Canvas {
public:
    Canvas(int width, int height, Color background = Palette::White);

    int width() const { return width_; }
    int height() const { return height_; }

    bool inBounds(int x, int y) const { return x >= 0 && x < width_ && y >= 0 && y < height_; }

    void clear(Color c);
    void setPixel(int x, int y, Color c);   // silently ignores out-of-bounds pixels
    Color getPixel(int x, int y) const;     // returns black when out of bounds

    // Writes a 24-bit uncompressed BMP. Returns false on failure.
    bool saveBMP(const std::string& path) const;

    const std::vector<Color>& pixels() const { return pixels_; }

private:
    int width_, height_;
    std::vector<Color> pixels_;  // row-major, row 0 is the bottom row
};
