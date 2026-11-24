// Still images that check each algorithm, component and scene on its own.
#include "Stills.h"
#include <cmath>
#include <filesystem>
#include <iostream>
#include "../algorithms/Circle.h"
#include "../algorithms/Curve.h"
#include "../algorithms/Ellipse.h"
#include "../algorithms/Fill.h"
#include "../algorithms/Line.h"
#include "../animation/Animation.h"
#include "../characters/Boyfriend.h"
#include "../characters/OtherMan.h"
#include "../characters/Thief.h"
#include "../characters/Woman.h"
#include "../core/Canvas.h"
#include "../objects/Door.h"
#include "../objects/Flower.h"
#include "../objects/GiftBox.h"
#include "../objects/Phone.h"
#include "../objects/Window.h"
#include "../scenes/HouseExterior.h"
#include "../scenes/HouseInterior.h"
#include "../scenes/Scene1Park.h"
#include "../scenes/Street.h"
#include "../text/PixelFont.h"
#include "../transform/Transform2D.h"

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

// Translation, rotation and scaling of the same polygon, plus the gift box
// rotated as it will be when it falls.
static void renderTransformTest(const std::string& path) {
    Canvas c(kWidth, kHeight, Palette::White);
    const std::vector<PointF> house = {{-40, 0}, {40, 0}, {40, 50}, {0, 85}, {-40, 50}};
    const PointF centre{0, 40};

    for (int i = 0; i < 5; ++i) {
        float t = i / 4.0f;
        Color fill = lerp(Palette::ShirtBlue, Palette::Red, t);

        // Row 1: translation only, stepping right and up
        auto moved = transformPolygon(translate(100.0f + i * 170, 540.0f + i * 15), house);
        scanlineFill(c, moved, fill);
        drawPolygon(c, moved, Palette::Black);

        // Row 2: rotation about the shape's centre, 0 to 180 degrees
        auto turned = transformPolygon(translate(100.0f + i * 170, 330.0f) * rotateAbout(i * 45.0f, centre), house);
        scanlineFill(c, turned, fill);
        drawPolygon(c, turned, Palette::Black);

        // Row 3: scaling about the base, 0.5x to 1.5x
        float s = 0.5f + t;
        auto scaled = transformPolygon(translate(100.0f + i * 170, 80.0f) * scaleAbout(s, s, {0, 0}), house);
        scanlineFill(c, scaled, fill);
        drawPolygon(c, scaled, Palette::Black);
    }

    // Gift box at four angles
    for (int i = 0; i < 4; ++i) drawGiftBox(c, 1080 + (i % 2) * 130, 420 - (i / 2) * 260, 100, i * 30.0f);
    save(c, path);
}

// Eight frames of one walk cycle (woman) and one run cycle (thief), side by side.
static void renderMotionTest(const std::string& path) {
    Canvas c(kWidth, kHeight, Palette::Backdrop);
    fillRect(c, 0, 360, kWidth, 4, Palette::Floor);
    fillRect(c, 0, 0, kWidth, 30, Palette::Floor);
    for (int i = 0; i < 8; ++i) {
        int x = 90 + i * 155;
        drawWoman(c, x, 364, Expression::Neutral, 0.95f, walkCycle(i / 8.0f / 0.9f, 1));
        drawThief(c, x, 30, Expression::Angry, 0.95f, runCycle(i / 8.0f / 1.6f, 1));
    }
    save(c, path);
}

// Every character in the pixel font, at three sizes.
static void renderFontTest(const std::string& path) {
    Canvas c(kWidth, kHeight, Palette::White);
    drawText(c, 40, 620, "ABCDEFGHIJKLMNOPQRSTUVWXYZ", 5, Palette::Black);
    drawText(c, 40, 540, "abcdefghijklmnopqrstuvwxyz", 5, Palette::Black);
    drawText(c, 40, 460, "0123456789 .,:'-!?", 5, Palette::Black);
    drawText(c, 40, 340, "Sometimes, life changes in a single day.", 4, Palette::Red);
    drawText(c, 40, 260, "Behind the Smile - Scene 1: The Relationship", 3, Palette::ShirtBlue);
    drawText(c, 40, 200, "Quick jumping fox, lazy dog? yes!", 2, Palette::Black);
    save(c, path);
}

void renderStills() {
    std::filesystem::create_directories("output");
    renderAlgorithmTest("output/algorithms_test.bmp");
    renderScene1("output/scene1_park.bmp");
    renderCharacterLineup("output/characters_lineup.bmp");
    renderExpressions("output/expressions.bmp");
    renderObjects("output/objects.bmp");
    renderBackgrounds();
    renderScene1Composite("output/scene1_composite.bmp");
    renderTransformTest("output/transforms_test.bmp");
    renderMotionTest("output/motion_test.bmp");
    renderFontTest("output/font_test.bmp");
}
