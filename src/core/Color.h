#pragma once
#include <cstdint>

struct Color {
    uint8_t r = 0, g = 0, b = 0;

    bool operator==(const Color& o) const { return r == o.r && g == o.g && b == o.b; }
    bool operator!=(const Color& o) const { return !(*this == o); }
};

// Linear blend between two colours, t in [0, 1]. Used for gradients and fades.
inline Color lerp(Color a, Color b, float t) {
    if (t < 0) t = 0;
    if (t > 1) t = 1;
    return Color{
        static_cast<uint8_t>(a.r + (b.r - a.r) * t),
        static_cast<uint8_t>(a.g + (b.g - a.g) * t),
        static_cast<uint8_t>(a.b + (b.b - a.b) * t),
    };
}

// Shared palette so every scene and character uses the same colours.
namespace Palette {
    constexpr Color White      {255, 255, 255};
    constexpr Color Black      {  0,   0,   0};
    constexpr Color SkyTop     { 90, 160, 230};
    constexpr Color SkyBottom  {190, 225, 250};
    constexpr Color Sun        {255, 215,  80};
    constexpr Color Cloud      {245, 248, 252};
    constexpr Color Grass      { 95, 175,  80};
    constexpr Color GrassDark  { 70, 140,  60};
    constexpr Color Path       {210, 185, 140};
    constexpr Color Bark       {110,  75,  45};
    constexpr Color Leaves     { 50, 125,  55};
    constexpr Color Wood       {150, 100,  60};
    constexpr Color Red        {220,  50,  60};
    constexpr Color Pink       {245, 140, 180};
    constexpr Color Yellow     {250, 220,  60};
    constexpr Color Purple     {150,  90, 200};
    constexpr Color Skin       {200, 140, 100};
    constexpr Color SkinDark   {120,  80,  55};
}
