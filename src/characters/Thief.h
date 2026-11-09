#pragma once
#include "../core/Canvas.h"
#include "Body.h"
#include "Expression.h"

// The stranger who steals her phone: thin, dark hoodie with the hood up,
// dark trousers and white trainers.
// (x, y) is the point between the feet. Returns the joints for attaching props.
Skeleton drawThief(Canvas& canvas, int x, int y, Expression e, float scale = 1.0f, const Pose& pose = Pose{});
