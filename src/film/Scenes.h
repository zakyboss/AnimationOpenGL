#pragma once
#include "../core/Canvas.h"

// Where the dropped gift comes to rest in the living-room doorway (Scenes 2
// and 3): bottom centre, size, and lying on its side.
constexpr int kGiftRestX = 1093, kGiftRestY = 212, kGiftSize = 42;
constexpr float kGiftRestAngle = -90.0f;

// One function per scene. t is the time in seconds since that scene started,
// so each scene can be written and tested on its own. The timeline in
// Film.cpp decides when each scene plays and adds the fades between them.
void playScene1(Canvas& canvas, float t);  // The Relationship   (park, afternoon)
void playScene2(Canvas& canvas, float t);  // The Discovery      (his house at sunset, outside then inside)
void playScene3(Canvas& canvas, float t);  // The Confrontation  (inside, evening)
void playScene4(Canvas& canvas, float t);  // The Journey Home   (street, night)
void playScene5(Canvas& canvas, float t);  // The Phone Theft    (street, night)
void playScene6(Canvas& canvas, float t);  // The Ending         (park bench at night, closing message)
