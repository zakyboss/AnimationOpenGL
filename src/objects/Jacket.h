#pragma once
#include "../characters/Body.h"
#include "../core/Canvas.h"

// The boyfriend's jacket, which he gives to her in the park.

// The body of a worn jacket: two front panels open down the middle (or one
// solid back when seen from behind). Drawn over the torso, before the arms.
void drawJacketBody(Canvas& canvas, const Skeleton& sk, bool fromBehind);

// Sleeves (worn properly) or just shoulders (draped over the shoulders).
// Drawn after the arms. skin redraws the hands at the ends of the sleeves.
void drawJacketSleeves(Canvas& canvas, const Skeleton& sk, bool sleeves, Color skin);

// Folded over a hand, hanging down; angle rotates it about the hand.
void drawJacketHeld(Canvas& canvas, Point hand, float scale, float angle = 0.0f);

// Crumpled on the floor; (x, y) is where it lies.
void drawJacketOnFloor(Canvas& canvas, int x, int y, float scale);
