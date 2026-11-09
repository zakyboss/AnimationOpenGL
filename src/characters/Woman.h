#pragma once
#include "../core/Canvas.h"
#include "Body.h"
#include "Expression.h"

// The main character: long dark hair, red dress, gold earrings.
// (x, y) is the point between the feet. Returns the joints for attaching props.
Skeleton drawWoman(Canvas& canvas, int x, int y, Expression e, float scale = 1.0f, const Pose& pose = Pose{});

// Just her head (hair, face, earrings), for close-ups and the expression sheet.
void drawWomanHead(Canvas& canvas, int cx, int cy, int r, Expression e);
