#include "Synth.h"
#include <algorithm>
#include <cmath>
#include <cstdint>

namespace {

constexpr float kPi = 3.14159265f;

// Repeatable white noise (a linear congruential generator), -1 to 1
struct Noise {
    uint32_t state = 12345;
    float next() {
        state = state * 1664525u + 1013904223u;
        return static_cast<int32_t>(state) / 2147483648.0f;
    }
};

// One-pole low-pass filter: smooths noise into a softer, duller sound
struct LowPass {
    float y = 0;
    float run(float x, float amount) { return y += amount * (x - y); }
};

// A drum-like hit: a sine whose pitch falls from f0 to f1, dying away
void addBoom(std::vector<float>& s, int rate, float at, float f0, float f1, float decay, float level) {
    float phase = 0;
    for (size_t i = static_cast<size_t>(at * rate); i < s.size(); ++i) {
        float t = i / float(rate) - at;
        float f = f1 + (f0 - f1) * std::exp(-t * 18);
        phase += 2 * kPi * f / rate;
        s[i] += level * std::sin(phase) * std::exp(-t / decay);
    }
}

// A burst of filtered noise; brightness 0 to 1 sets how sharp it sounds
void addNoise(std::vector<float>& s, int rate, float at, float decay, float brightness, float level, Noise& noise) {
    LowPass lp;
    for (size_t i = static_cast<size_t>(at * rate); i < s.size(); ++i) {
        float t = i / float(rate) - at;
        if (t > decay * 6) break;
        s[i] += level * lp.run(noise.next(), brightness) * std::exp(-t / decay);
    }
}

// A swell of noise that rises and falls, its tone sweeping up then down
void addWhoosh(std::vector<float>& s, int rate, float at, float length, float level, Noise& noise) {
    LowPass lp1, lp2;
    for (size_t i = static_cast<size_t>(at * rate); i < s.size(); ++i) {
        float u = (i / float(rate) - at) / length;
        if (u > 1) break;
        float shape = std::sin(kPi * u);
        float x = lp2.run(lp1.run(noise.next(), 0.05f + 0.25f * shape), 0.3f);
        s[i] += level * 3.0f * x * shape * shape;
    }
}

}  // namespace

bool synthesize(const std::string& name, int rate, std::vector<short>& out) {
    Noise noise;
    std::vector<float> s;
    auto length = [&](float seconds) { s.assign(static_cast<size_t>(seconds * rate), 0.0f); };

    if (name == "sting") {
        length(3.2f);
        addBoom(s, rate, 0.0f, 110, 38, 0.9f, 0.9f);       // the impact
        addNoise(s, rate, 0.0f, 0.12f, 0.15f, 0.6f, noise);
        addBoom(s, rate, 1.05f, 70, 45, 0.18f, 0.55f);     // heartbeat: lub...
        addBoom(s, rate, 1.33f, 65, 42, 0.16f, 0.4f);      // ...dub
        // A sour, ringing chord (two notes a semitone apart beat against each other)
        for (size_t i = 0; i < s.size(); ++i) {
            float t = i / float(rate);
            float ring = std::sin(2 * kPi * 622.25f * t) + std::sin(2 * kPi * 659.26f * t) + 0.5f * std::sin(2 * kPi * 311.13f * t);
            s[i] += 0.07f * ring * std::min(1.0f, t * 20) * std::exp(-t / 1.1f);
        }
    } else if (name == "thud") {
        length(0.6f);
        addBoom(s, rate, 0.0f, 160, 70, 0.07f, 0.9f);
        addNoise(s, rate, 0.0f, 0.02f, 0.5f, 0.5f, noise);
        addBoom(s, rate, 0.19f, 170, 80, 0.04f, 0.35f);    // a little bounce
        addNoise(s, rate, 0.19f, 0.012f, 0.5f, 0.2f, noise);
    } else if (name == "crunch") {
        length(0.7f);
        addBoom(s, rate, 0.0f, 130, 55, 0.08f, 0.8f);      // the foot coming down
        // Cardboard crackling as it folds: many tiny sharp bursts, fading out
        for (int k = 0; k < 26; ++k) {
            float at = 0.01f + 0.3f * std::pow(k / 26.0f, 1.6f) + 0.006f * noise.next();
            addNoise(s, rate, std::max(0.0f, at), 0.008f, 0.6f, 0.55f * (1.0f - k / 30.0f), noise);
        }
    } else if (name == "whoosh") {
        length(0.55f);
        addWhoosh(s, rate, 0.0f, 0.5f, 0.9f, noise);
    } else if (name == "shove") {
        length(0.9f);
        addWhoosh(s, rate, 0.0f, 0.25f, 0.6f, noise);
        addBoom(s, rate, 0.12f, 120, 60, 0.06f, 0.7f);     // hands hit his chest
        addNoise(s, rate, 0.12f, 0.03f, 0.3f, 0.4f, noise);
        for (float at : {0.38f, 0.55f, 0.68f}) {            // stumbling steps
            addBoom(s, rate, at, 140, 90, 0.03f, 0.35f);
            addNoise(s, rate, at, 0.015f, 0.4f, 0.25f, noise);
        }
    } else {
        return false;
    }

    // Short fades at both ends so there are no clicks, then to 16-bit stereo
    const size_t edge = rate / 200;
    out.resize(s.size() * 2);
    for (size_t i = 0; i < s.size(); ++i) {
        float fade = std::min({1.0f, i / float(edge), (s.size() - i) / float(edge)});
        short v = static_cast<short>(std::clamp(s[i] * fade, -1.0f, 1.0f) * 32000.0f);
        out[2 * i] = out[2 * i + 1] = v;
    }
    return true;
}
