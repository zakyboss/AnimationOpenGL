#pragma once
#include "../core/Canvas.h"

// The surprise gift; (x, y) is the bottom centre, size is the box width.
// angle rotates the whole box about its centre (degrees, anticlockwise),
// which is used when it falls in Scene 2. squash from 0 to 1 flattens it
// onto the floor (scaling it down vertically and out sideways), for when it
// is stamped on in Scene 3.
void drawGiftBox(Canvas& canvas, int x, int y, int size, float angle = 0.0f, float squash = 0.0f);
