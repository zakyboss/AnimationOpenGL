#pragma once
#include "../core/Canvas.h"

// A smartphone held upright; (x, y) is its centre. 26 x 48 px at scale 1.
// screenToUs shows the screen; otherwise we see the back of the phone (case,
// camera lenses), as when someone in the picture is looking at its screen.
void drawPhone(Canvas& canvas, int x, int y, float scale = 1.0f, bool screenToUs = true);
