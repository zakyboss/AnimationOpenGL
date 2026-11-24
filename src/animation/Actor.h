#pragma once
#include <vector>
#include "../characters/Body.h"
#include "../characters/Expression.h"

// A script for one character in a scene: where they are, how they hold
// their arms, and what face they make, all as functions of time.

enum class Gait { Walk, Run };

// Which arms swing while walking or running; the others keep the pose from
// the pose keys (for example an arm holding a hand, a phone or a gift).
enum Swing { SwingNone = 0, SwingLeft = 1, SwingRight = 2, SwingBoth = 3 };

// Between two waypoints the character moves in a straight line (translation)
// and its legs follow the walk or run cycle. The settings on the second
// waypoint describe that move. Two waypoints with the same time make the
// character jump straight there (for example, re-entering from the other side).
struct Waypoint {
    float time;
    float x, y;
    float scale = 1.0f;
    Gait gait = Gait::Walk;
    int swing = SwingBoth;
    float pace = 0.0f;  // 0 works the pace out from the distance, so the feet don't slide
};

// From this time on the character blends into this pose.
struct PoseKey {
    float time;
    Pose pose;
};

// From this time on the character has this expression.
struct FaceKey {
    float time;
    Expression face;
};

// From this time on the eyes turn towards (x, y), each -1 to 1.
struct GazeKey {
    float time;
    float x, y;
};

// A stretch of time when the character is talking (the mouth moves).
struct Span {
    float start, end;
};

struct Track {
    std::vector<Waypoint> path;
    std::vector<PoseKey> poses;
    std::vector<FaceKey> faces;
    std::vector<GazeKey> gazes;
    std::vector<Span> talk;
};

// Everything needed to draw the character at one moment.
struct ActorFrame {
    int x = 0, y = 0;
    float scale = 1.0f;
    Pose pose;
    Expression face = Expression::Neutral;
};

ActorFrame actorAt(const Track& track, float t);
