#pragma once
#include "../core/Canvas.h"

// Small moving details that make the world feel alive.

// A bird seen from far away: two curved wings. flap 0 to 1 moves the wings
// from up to down.
void drawBird(Canvas& canvas, int x, int y, int size, float flap);

// A small car seen from the side, driving right; (x, y) is the bottom centre.
// With headlights on it throws a beam ahead of it.
void drawCar(Canvas& canvas, int x, int y, float scale, Color body, bool headlights);

// A lit cigarette held at (x, y), with smoke puffs rising from it over time.
void drawCigarette(Canvas& canvas, int x, int y, float time);
