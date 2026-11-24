#pragma once
#include "../core/Canvas.h"
#include "Body.h"
#include "Expression.h"

// Background extras (people walking, a jogger), built on the same skeleton
// as the main characters but with their look chosen per person.

enum class HairStyle { Short, Long, Bald, Cap };

struct PasserbyLook {
    Color skin, hair, top, bottom, shoes;
    HairStyle hairStyle = HairStyle::Short;
    bool skirt = false;
};

// (x, y) is the point between the feet. Returns the joints.
Skeleton drawPasserby(Canvas& canvas, int x, int y, const PasserbyLook& look, Expression e, float scale = 1.0f,
                      const Pose& pose = Pose{});
