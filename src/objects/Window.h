#pragma once
#include "../core/Canvas.h"

// A four-pane window with a sill; (x, y) is the bottom-left corner of the glass.
// glass sets what shows through it (daylight, dusk, a lit room).
void drawWindow(Canvas& canvas, int x, int y, int w, int h, bool curtains = false,
                Color glass = Palette::Glass);
