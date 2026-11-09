// Behind the Smile: Phase 2 (pure C++ graphics).
// Renders still images into output/ to check each algorithm, component and scene.
#include <cmath>
#include <filesystem>
#include <iostream>
#include "algorithms/Circle.h"
#include "algorithms/Curve.h"
#include "algorithms/Ellipse.h"
#include "algorithms/Fill.h"
#include "algorithms/Line.h"
#include "characters/Boyfriend.h"
#include "characters/OtherMan.h"
#include "characters/Thief.h"
#include "characters/Woman.h"
#include "core/Canvas.h"
#include "objects/Door.h"
#include "objects/Flower.h"
#include "objects/GiftBox.h"
#include "objects/Phone.h"
#include "objects/Window.h"
#include "scenes/HouseExterior.h"
#include "scenes/HouseInterior.h"
#include "scenes/Scene1Park.h"
#include "scenes/Street.h"

constexpr int kWidth = 1280;
constexpr int kHeight = 720;

static void save(const Canvas& c, const std::string& path) {
    if (c.saveBMP(path)) std::cout << "Saved " << path << "\n";
    else std::cerr << "Failed to save " << path << "\n";
}

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

    save(c, path);
}

static void renderCharacterLineup(const std::string& path) {
    Canvas c(kWidth, kHeight, Palette::Backdrop);
    fillRect(c, 0, 0, kWidth, 70, Palette::Floor);
    const float scale = 1.35f;
    drawWoman(c, 200, 70, Expression::Neutral, scale);
    drawBoyfriend(c, 500, 70, Expression::Neutral, scale);
    drawOtherMan(c, 800, 70, Expression::Neutral, scale);
    drawThief(c, 1100, 70, Expression::Neutral, scale);
    save(c, path);
}

static void renderExpressions(const std::string& path) {
    Canvas c(kWidth, kHeight, Palette::Backdrop);
    const Expression all[] = {Expression::Happy, Expression::Neutral, Expression::Shocked,
                              Expression::Angry, Expression::Sad};
    for (int i = 0; i < 5; ++i) drawWomanHead(c, 140 + i * 250, 400, 90, all[i]);
    save(c, path);
}

static void renderObjects(const std::string& path) {
    Canvas c(kWidth, kHeight, Palette::Backdrop);
    fillRect(c, 0, 0, kWidth, 80, Palette::Floor);

    // Top row: single flowers and the bouquet
    const Color petals[] = {Palette::Red, Palette::Pink, Palette::Purple, Palette::Yellow};
    for (int i = 0; i < 4; ++i) drawFlower(c, 620 + i * 110, 600, petals[i], 1.8f);
    drawBouquet(c, 300, 450, 1.4f);

    // Bottom row: gift, phone, doors (closed and open), window
    drawGiftBox(c, 140, 80, 170);
    drawPhone(c, 380, 230, 3.0f);
    drawDoor(c, 540, 80, 140, 280);
    drawDoor(c, 820, 80, 140, 280, true);
    drawWindow(c, 1020, 160, 190, 190, true);
    save(c, path);
}

static void renderBackgrounds() {
    Canvas c(kWidth, kHeight);
    drawHouseExterior(c);
    save(c, "output/scene2_house_exterior.bmp");
    drawHouseInterior(c);
    save(c, "output/scene3_house_interior.bmp");
    drawStreet(c, 0.15f);
    save(c, "output/scene4_street.bmp");
    drawStreet(c, 0.55f);
    save(c, "output/scene5_street.bmp");
    drawStreet(c, 1.0f);
    save(c, "output/scene6_street_night.bmp");
}

// The couple standing together in the park, holding hands, with flowers.
static void renderScene1Composite(const std::string& path) {
    Canvas c(kWidth, kHeight);
    drawParkBackground(c);

    Pose woman;
    woman.shoulder[Left] = -10;   // left arm bent to hold the bouquet
    woman.elbow[Left] = 80;
    woman.shoulder[Right] = 40;   // right arm reaching to hold his hand
    Pose boyfriend;
    boyfriend.shoulder[Left] = -26;

    Skeleton her = drawWoman(c, 560, 70, Expression::Happy, 1.0f, woman);
    drawBoyfriend(c, 721, 70, Expression::Happy, 1.0f, boyfriend);
    drawBouquet(c, her.hand[Left].x, her.hand[Left].y, 0.75f);
    save(c, path);
}

static void renderScene1(const std::string& path) {
    Canvas c(kWidth, kHeight);
    drawParkBackground(c);
    save(c, path);
}

int main() {
    std::filesystem::create_directories("output");
    renderAlgorithmTest("output/algorithms_test.bmp");
    renderScene1("output/scene1_park.bmp");
    renderCharacterLineup("output/characters_lineup.bmp");
    renderExpressions("output/expressions.bmp");
    renderObjects("output/objects.bmp");
    renderBackgrounds();
    renderScene1Composite("output/scene1_composite.bmp");
    return 0;
}
