#pragma once
#include "../core/Canvas.h"
#include "Body.h"
#include "Expression.h"

// The man involved with the boyfriend: stocky, curly brown hair, glasses,
// maroon shirt and khaki trousers.
// (x, y) is the point between the feet. Returns the joints for attaching props.
Skeleton drawOtherMan(Canvas& canvas, int x, int y, Expression e, float scale = 1.0f, const Pose& pose = Pose{});
