#include "Actor.h"
#include <algorithm>
#include <cmath>
#include "Animation.h"

namespace {

constexpr float kPoseBlend = 0.4f;  // seconds to move into a new pose
constexpr float kGaitBlend = 0.2f;  // seconds to ease into and out of a stride

Pose poseAt(const std::vector<PoseKey>& keys, float t) {
    if (keys.empty() || t < keys.front().time) return keys.empty() ? Pose{} : keys.front().pose;
    size_t i = 0;
    while (i + 1 < keys.size() && keys[i + 1].time <= t) ++i;
    if (i == 0) return keys[0].pose;
    return lerpPose(keys[i - 1].pose, keys[i].pose, easeInOut(progress(t, keys[i].time, keys[i].time + kPoseBlend)));
}

Expression faceAt(const std::vector<FaceKey>& keys, float t) {
    Expression face = keys.empty() ? Expression::Neutral : keys.front().face;
    for (const FaceKey& k : keys)
        if (k.time <= t) face = k.face;
    return face;
}

// The eyes move quickly to each new target.
void gazeAt(const std::vector<GazeKey>& keys, float t, float& x, float& y) {
    x = y = 0.0f;
    if (keys.empty()) return;
    size_t i = 0;
    while (i + 1 < keys.size() && keys[i + 1].time <= t) ++i;
    if (t < keys[0].time || i == 0) {
        x = keys[0].x;
        y = keys[0].y;
        return;
    }
    float u = easeInOut(progress(t, keys[i].time, keys[i].time + 0.25f));
    x = lerpf(keys[i - 1].x, keys[i].x, u);
    y = lerpf(keys[i - 1].y, keys[i].y, u);
}

// While talking the mouth opens and closes in an uneven rhythm, like syllables.
float mouthAt(const std::vector<Span>& talk, float t) {
    for (const Span& s : talk) {
        if (t >= s.start && t < s.end) {
            // Two waves at different speeds: some syllables open wide, some
            // barely, with short closes in between
            float v = std::sin(t * 17.0f) * 0.6f + std::sin(t * 6.3f + 1.0f) * 0.4f;
            float open = std::min(1.0f, 0.5f + 0.6f * v);
            return open > 0.18f ? open : 0.0f;
        }
    }
    return 0.0f;
}

}  // namespace

ActorFrame actorAt(const Track& track, float t) {
    ActorFrame f;
    f.pose = poseAt(track.poses, t);
    f.face = faceAt(track.faces, t);
    gazeAt(track.gazes, t, f.pose.gazeX, f.pose.gazeY);
    f.pose.mouth = mouthAt(track.talk, t);

    // Each character blinks every few seconds, at their own moment
    const float seed = track.path.empty() ? 0.0f : std::fabs(track.path.front().x) * 0.013f;
    f.pose.blink = std::fmod(t + seed, 3.7f + std::fmod(seed, 0.9f)) < 0.13f ? 1.0f : 0.0f;

    const std::vector<Waypoint>& path = track.path;
    if (path.empty()) return f;

    // Before the first or after the last waypoint, stand still there
    const Waypoint* a = &path.front();
    const Waypoint* b = a;
    if (t >= path.back().time) {
        a = b = &path.back();
    } else {
        for (size_t i = 0; i + 1 < path.size(); ++i) {
            if (path[i].time <= t && t < path[i + 1].time) {
                a = &path[i];
                b = &path[i + 1];
                break;
            }
        }
    }

    const float duration = b->time - a->time;
    const float local = t - a->time;
    const float u = duration > 0 ? local / duration : 0.0f;
    const float x = lerpf(a->x, b->x, u), y = lerpf(a->y, b->y, u);
    f.scale = lerpf(a->scale, b->scale, u);
    f.x = static_cast<int>(std::lround(x));
    f.y = static_cast<int>(std::lround(y));

    const float distance = std::hypot(b->x - a->x, b->y - a->y);
    if (duration <= 0 || distance < 1.0f) {
        // Standing: the arms sway very slightly, so nobody is ever frozen
        for (int i = 0; i < 2; ++i) f.pose.shoulder[i] += 1.8f * std::sin(t * 1.3f + seed + i * 1.7f);
        return f;
    }

    // Moving: swap the legs (and swinging arms) for the walk or run cycle
    const bool run = b->gait == Gait::Run;
    const float strideSpeed = (run ? kRunSpeed : kWalkSpeed) * f.scale;
    const float pace = b->pace > 0 ? b->pace : distance / duration / strideSpeed;
    const int direction = b->x >= a->x ? 1 : -1;
    // Moving up or down the screen means walking away from or towards the viewer
    const float lateral = std::fabs(b->x - a->x) / distance;
    const Pose cycle = run ? runCycle(local, direction, pace, lateral) : walkCycle(local, direction, pace, lateral);

    // Ease in and out of the stride so starting and stopping don't snap
    const float w = clamp01(std::min(local, duration - local) / kGaitBlend);
    for (int i = 0; i < 2; ++i) {
        f.pose.hip[i] = lerpf(f.pose.hip[i], cycle.hip[i], w);
        f.pose.knee[i] = lerpf(f.pose.knee[i], cycle.knee[i], w);
        f.pose.lift[i] = lerpf(f.pose.lift[i], cycle.lift[i], w);
        if (b->swing & (i == Left ? SwingLeft : SwingRight)) {
            f.pose.shoulder[i] = lerpf(f.pose.shoulder[i], cycle.shoulder[i], w);
            f.pose.elbow[i] = lerpf(f.pose.elbow[i], cycle.elbow[i], w);
        }
    }
    return f;
}
