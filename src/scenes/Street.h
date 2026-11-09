#pragma once
#include "../core/Canvas.h"

// Scenes 4 to 6: the street on her way home. darkness runs from 0 (late
// afternoon) to 1 (night), so the light can fade across the three scenes.
// Characters walk along the pavement (y around 120 to 170).
void drawStreet(Canvas& canvas, float darkness = 0.0f);
