// Behind the Smile: Phase 2 (pure C++ graphics).
// Renders still images into output/ to check each algorithm and scene.
#include <cmath>
#include <filesystem>
#include <iostream>
#include "algorithms/Circle.h"
#include "algorithms/Curve.h"
#include "algorithms/Ellipse.h"
#include "algorithms/Fill.h"
#include "algorithms/Line.h"
#include "core/Canvas.h"
#include "scenes/Scene1Park.h"

constexpr int kWidth = 1280;
constexpr int kHeight = 720;

// One panel per algorithm, so the checkpoint can show each working on its own.
static void renderAlgorithmTest(const std::string& path) {
    Canvas c(kWidth, kHeight, Palette::White);

    // Lines: DDA (blue) vs Bresenham (red) fanning out in every octant
    for (int angle = 0; angle < 360; angle += 15) {
        float rad = angle * 3.14159265f / 180.0f;
        int ex = 170 + static_cast<int>(120 * std::cos(rad));
        int ey = 540 + static_cast<int>(120 * std::sin(rad));
        if (angle % 30 == 0)
            drawLineDDA(c, 170, 540, ex, ey, Color{40, 80, 200});
        else
            drawLineBresenham(c, 170, 540, ex, ey, Palette::Red);
    }

    // Midpoint circles: outline and filled
    for (int r = 20; r <= 120; r += 25) drawCircle(c, 480, 540, r, Palette::Black);
    fillCircle(c, 480, 540, 15, Palette::Red);

    // Midpoint ellipses: outline and filled
    drawEllipse(c, 800, 540, 150, 70, Palette::Black);
    drawEllipse(c, 800, 540, 60, 110, Palette::Black);
    fillEllipse(c, 800, 540, 40, 20, Palette::Purple);

    // Bezier curves
    drawQuadBezier(c, {1000, 460}, {1110, 680}, {1220, 460}, Palette::Black, 3);
    drawCubicBezier(c, {1000, 540}, {1060, 420}, {1160, 660}, {1220, 540}, Palette::Red, 2);

    // Scanline fill on a concave polygon (a star)
    std::vector<Point> star;
    for (int i = 0; i < 10; ++i) {
        float a = 3.14159265f / 2 + i * 3.14159265f / 5;
        int r = i % 2 == 0 ? 120 : 50;
        star.push_back({170 + static_cast<int>(r * std::cos(a)), 200 + static_cast<int>(r * std::sin(a))});
    }
    scanlineFill(c, star, Palette::Yellow);
    drawPolygon(c, star, Palette::Black);

    // Boundary fill inside an outlined circle
    drawCircle(c, 480, 200, 100, Palette::Black);
    boundaryFill(c, 480, 200, Palette::Pink, Palette::Black);

    // Flood fill one region of a split rectangle
    drawPolygon(c, {{680, 110}, {920, 110}, {920, 290}, {680, 290}}, Palette::Black);
    drawLineBresenham(c, 680, 110, 920, 290, Palette::Black);
    floodFill(c, 860, 140, Palette::Grass);

    // Thick lines, as used for limbs
    for (int t = 1; t <= 9; t += 2)
        drawThickLine(c, 1000 + t * 20, 110, 1040 + t * 20, 290, t, Palette::Bark);

    if (c.saveBMP(path)) std::cout << "Saved " << path << "\n";
    else std::cerr << "Failed to save " << path << "\n";
}

static void renderScene1(const std::string& path) {
    Canvas c(kWidth, kHeight);
    drawParkBackground(c);
    if (c.saveBMP(path)) std::cout << "Saved " << path << "\n";
    else std::cerr << "Failed to save " << path << "\n";
}

int main() {
    std::filesystem::create_directories("output");
    renderAlgorithmTest("output/algorithms_test.bmp");
    renderScene1("output/scene1_park.bmp");
    return 0;
}
