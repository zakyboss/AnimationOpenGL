#pragma once
#include "../core/Canvas.h"
#include "Body.h"
#include "Expression.h"

// Her partner: tall, short black hair, blue shirt and jeans.
// (x, y) is the point between the feet. Returns the joints for attaching props.
Skeleton drawBoyfriend(Canvas& canvas, int x, int y, Expression e, float scale = 1.0f, const Pose& pose = Pose{});
