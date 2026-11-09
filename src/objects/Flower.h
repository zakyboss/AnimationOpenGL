#pragma once
#include "../core/Canvas.h"

// A single flower; (x, y) is the centre of the head, the stem hangs below it.
void drawFlower(Canvas& canvas, int x, int y, Color petal, float scale = 1.0f);

// A wrapped bunch of flowers; (x, y) is the bottom tip, where a hand holds it.
void drawBouquet(Canvas& canvas, int x, int y, float scale = 1.0f);
