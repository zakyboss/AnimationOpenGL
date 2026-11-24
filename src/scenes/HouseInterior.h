#pragma once
#include "../core/Canvas.h"

// Scenes 2 and 3: the boyfriend's living room. Characters stand on the rug
// (y around 60 to 120); someone in the doorway stands at about (1100, 214).
// In the evening the window shows the dusk sky and the lamp is lit.
// The sofa seat is at about y = 305, between x = 520 and 780.
void drawHouseInterior(Canvas& canvas, bool doorOpen = false, bool evening = false);
