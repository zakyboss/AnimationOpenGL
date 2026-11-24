#pragma once
#include "../core/Canvas.h"

// The park fountain: a stone basin, a pillar with a bowl on top, and water
// arcing down into the basin with droplets and ripples that move with time.
// (x, y) is the bottom centre of the basin. darkness 0 to 1 dims it at night.
void drawFountain(Canvas& canvas, int x, int y, float time, float darkness = 0.0f);

// A small bird perched at (x, y) taking a bath: it dips its head into the
// water, flutters its wings and sends up splashes.
void drawBathingBird(Canvas& canvas, int x, int y, float time);
