#pragma once
#include "../core/Canvas.h"

// A panelled front door; (x, y) is the bottom-left corner of the doorway.
// When open, the door swings out on its left hinge and the dark room shows.
void drawDoor(Canvas& canvas, int x, int y, int w, int h, bool open = false);
