#pragma once
#include <string>
#include "../core/Canvas.h"

// A 5 x 7 bitmap font drawn in pure C++: each lit cell of a glyph is a small
// filled square. Supports A-Z, a-z, 0-9, space and . , : ' - ! ?
// size is the width of one cell in pixels; (x, y) is the left end of the baseline.
void drawText(Canvas& canvas, int x, int y, const std::string& text, int size, Color color);

// Width of the text in pixels at the given size.
int textWidth(const std::string& text, int size);

// Draws the text centred horizontally on cx.
void drawTextCentred(Canvas& canvas, int cx, int y, const std::string& text, int size, Color color);
