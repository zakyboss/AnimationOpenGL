#pragma once
#include <vector>
#include "../app/Soundtrack.h"
#include "../core/Canvas.h"

constexpr int kFilmWidth = 1280;
constexpr int kFilmHeight = 720;
constexpr float kFilmFps = 30.0f;

// Length of the film in seconds.
float filmDuration();

// Start time of each chapter (scene), for jumping with the number keys.
std::vector<float> filmChapters();

// All the sound, placed on the film's timeline: music, the spoken lines
// (from the dialogue tables), background sound and effects (files in Resources/).
std::vector<SoundCue> filmSound();

// Draws the frame at the given time. Frames depend only on time, so any
// moment can be rendered on its own (for the player, or to a BMP for checking).
void renderFilm(Canvas& canvas, float seconds);
