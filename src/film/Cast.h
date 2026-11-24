#pragma once
#include <functional>
#include <vector>
#include "../animation/Actor.h"
#include "../characters/Passerby.h"
#include "../core/Canvas.h"

enum class Character { Woman, Boyfriend, OtherMan, Thief };

enum class Jacket { None, Worn, Draped };

// Draws a character from an ActorFrame, raised by dy pixels (for bounces,
// sobs and breathing), optionally wearing the jacket. Also finishes the
// hands-over-face movement when the pose covers the face. Returns the
// skeleton so props can follow the hands.
Skeleton drawActor(Canvas& canvas, Character who, const ActorFrame& f, int dy = 0, Jacket jacket = Jacket::None);

Skeleton drawExtra(Canvas& canvas, const PasserbyLook& look, const ActorFrame& f);

// Redraws just the arms, on top of whatever was drawn since (for a hug from behind).
void drawArmsOver(Canvas& canvas, Character who, const Skeleton& sk);

Color skinOf(Character who);

// Midpoint between the two hands, where held objects go.
Point betweenHands(const Skeleton& sk);

// Draws things from the back of the scene to the front: larger depth
// (higher up the screen, further away) first.
struct Drawable {
    int depth;
    std::function<void()> draw;
};
void drawByDepth(std::vector<Drawable>& items);
