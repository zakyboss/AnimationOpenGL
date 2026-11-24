#pragma once
#include <string>
#include "../core/Canvas.h"

// Blends every pixel of the canvas towards a colour. amount 0 leaves the
// frame unchanged, 1 makes it solid colour. Used for fades between scenes.
void fadeToColor(Canvas& canvas, Color color, float amount);

// White centred text with a dark drop shadow, readable on any background.
// opacity below 1 blends the text into the background colour behind it.
void drawCaption(Canvas& canvas, int cy, const std::string& text, int size, float opacity = 1.0f);
