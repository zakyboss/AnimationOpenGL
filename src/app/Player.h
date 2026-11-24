#pragma once
#include <functional>
#include <vector>
#include "../core/Canvas.h"
#include "Soundtrack.h"

using RenderFrame = std::function<void(Canvas&, float seconds)>;

// Opens a window and plays the film in real time, with its music. Each frame
// is drawn into the canvas by our own code; OpenGL only copies the finished
// pixels to the screen. chapters are the start times that the number keys jump to.
//
// Keys: Space pause/play, R restart, Left/Right skip 5 s, , and . step one
//       frame, 1-9 jump to a scene, M mute, Esc quit.
void runPlayer(int& argc, char** argv, Canvas& canvas, float duration, float fps,
               const std::vector<float>& chapters, const std::vector<SoundCue>& sound, RenderFrame render);
