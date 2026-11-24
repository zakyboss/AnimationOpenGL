#pragma once
#include "../core/Canvas.h"

// The park, used at the start (day) and at the end (night). darkness runs
// from 0 (sunny afternoon) to 1 (night: moon, stars and a lit lamp over the
// bench). With clouds = false the scene draws its own drifting clouds.
// The bench seat is at about y = 158, between x = 200 and 360.
void drawParkBackground(Canvas& canvas, float darkness = 0.0f, bool clouds = true);

// A cloud centred on (x, y), for drifting clouds drawn each frame.
void drawCloud(Canvas& canvas, int x, int y, int size, Color color = Palette::Cloud);
