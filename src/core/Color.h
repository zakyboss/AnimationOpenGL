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
    constexpr Color Backdrop   {232, 234, 240};
    constexpr Color Floor      {205, 208, 215};

    // Characters: skin and hair
    constexpr Color SkinLight  {236, 196, 160};
    constexpr Color SkinMedium {190, 130,  90};
    constexpr Color SkinDeep   {120,  78,  52};
    constexpr Color SkinTan    {165, 112,  76};
    constexpr Color HairDark   { 50,  32,  26};
    constexpr Color HairBlack  { 25,  20,  20};
    constexpr Color HairBrown  {110,  72,  40};

    // Characters: faces
    constexpr Color EyeDark    { 40,  30,  30};
    constexpr Color MouthDark  {110,  35,  40};
    constexpr Color LipRed     {185,  45,  70};
    constexpr Color Blush      {240, 140, 140};
    constexpr Color Tear       {120, 190, 240};
    constexpr Color Gold       {232, 182,  50};
    constexpr Color GlassesFrame{ 30,  30,  35};

    // Characters: clothing
    constexpr Color DressRed   {205,  55,  80};
    constexpr Color ShirtBlue  { 60, 115, 195};
    constexpr Color Denim      { 45,  62,  98};
    constexpr Color ShirtMaroon{130,  40,  58};
    constexpr Color Khaki      {185, 165, 115};
    constexpr Color Hoodie     { 58,  60,  68};
    constexpr Color HoodieShade{ 42,  44,  50};
    constexpr Color TrouserDark{ 30,  30,  36};
    constexpr Color Belt       { 60,  40,  25};
    constexpr Color ShoeRed    {165,  28,  48};
    constexpr Color ShoeBrown  { 92,  56,  30};
    constexpr Color ShoeBlack  { 28,  28,  30};
    constexpr Color Sneaker    {236, 236, 236};

    // Objects
    constexpr Color GiftBox    {120,  80, 200};
    constexpr Color GiftLid    {140, 100, 220};
    constexpr Color Ribbon     {245, 200,  60};
    constexpr Color RibbonShade{215, 165,  40};
    constexpr Color WrapPaper  {250, 240, 215};
    constexpr Color PhoneBody  { 32,  32,  38};
    constexpr Color PhoneScreen{ 95, 175, 235};
    constexpr Color PhoneCamera{ 80,  80,  90};
    constexpr Color DoorWood   {135,  82,  45};
    constexpr Color DoorPanel  {112,  66,  35};
    constexpr Color DoorFrame  {238, 232, 222};
    constexpr Color Doorway    { 52,  42,  38};
    constexpr Color WindowFrame{246, 246, 242};
    constexpr Color Glass      {172, 212, 236};
    constexpr Color GlassShine {215, 235, 248};
    constexpr Color Curtain    {196,  88,  92};

    // House
    constexpr Color WallCream  {242, 226, 190};
    constexpr Color Roof       {160,  62,  52};
    constexpr Color Brick      {165,  92,  72};
    constexpr Color Fence      {250, 250, 248};
    constexpr Color WallInside {228, 208, 176};
    constexpr Color FloorWood  {172, 122,  78};
    constexpr Color FloorLine  {142,  96,  58};
    constexpr Color Skirting   {246, 240, 230};
    constexpr Color Sofa       { 88, 122, 164};
    constexpr Color SofaDark   { 68,  96, 132};
    constexpr Color Rug        {178,  72,  72};
    constexpr Color RugInner   {210, 150, 110};
    constexpr Color LampShade  {250, 230, 180};
    constexpr Color Metal      {118, 118, 124};

    // Street
    constexpr Color DuskTop    { 78,  70, 140};
    constexpr Color DuskBottom {245, 160, 115};
    constexpr Color NightTop   { 14,  18,  44};
    constexpr Color NightBottom{ 52,  50,  92};
    constexpr Color BuildingA  {122, 110, 132};
    constexpr Color BuildingB  {150, 118, 108};
    constexpr Color BuildingC  { 98, 114, 138};
    constexpr Color Parapet    { 70,  68,  82};
    constexpr Color WindowLit  {255, 220, 130};
    constexpr Color WindowDark { 60,  66,  88};
    constexpr Color Pavement   {172, 172, 166};
    constexpr Color PavementLine{146, 146, 140};
    constexpr Color Kerb       {205, 205, 198};
    constexpr Color Road       { 64,  64,  70};
    constexpr Color LampPost   { 48,  52,  58};
    constexpr Color LampLight  {255, 236, 160};
}
