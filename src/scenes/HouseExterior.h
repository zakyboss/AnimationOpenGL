#pragma once
#include "../core/Canvas.h"

// Scene 2, "The Discovery": the boyfriend's house from the front garden.
// Characters stand on the garden path (y around 30 to 250). evening runs from
// 0 (day) to 1 (sunset, with the lights on inside). The left window's glass
// is at x 420-560, y 320-440.
void drawHouseExterior(Canvas& canvas, bool doorOpen = false, float evening = 0.0f);
