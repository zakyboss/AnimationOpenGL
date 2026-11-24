#include "Dialogue.h"
#include <algorithm>
#include "../algorithms/Circle.h"
#include "../algorithms/Fill.h"
#include "../text/PixelFont.h"

namespace {

using S = Speaker;

// Times are seconds into each scene; lengths were measured from the
// recordings (sound from first word to last, silence trimmed).
//
// Built on first use (inside a function) rather than as a global: the
// scenes' Tracks ask for these lines while the program is starting up, and
// C++ does not say which file's globals are created first.
const std::vector<Line>* allScenes() {
    static const std::vector<Line> kScenes[6] = {
    // Scene 1: The Relationship (park, afternoon)
    {
        {5.0f, S::Woman, "I can't believe it's been a whole year already.", "s1_01_woman", 1.97f},
        {7.2f, S::Boyfriend, "Best year of my life.", "s1_02_boyfriend", 1.12f},
        {8.6f, S::Woman, "Okay, so where are you taking me?", "s1_03_woman", 3.39f},
        {12.1f, S::Boyfriend, "You'll see.", "s1_04_boyfriend", 0.57f},
        {12.9f, S::Woman, "Brr... it's freezing!", "s1_05_woman", 2.94f},
        {15.9f, S::Boyfriend, "Here, take my jacket.", "s1_06_boyfriend", 1.03f},
        {20.5f, S::Woman, "You always know how to keep me warm.", "s1_07_woman", 2.13f},
        {24.4f, S::Boyfriend, "Close your eyes. No peeking.", "s1_08_boyfriend", 1.62f},
        {33.6f, S::Woman, "Oh my God... they're beautiful!", "s1_09_woman", 2.69f},
        {36.4f, S::Boyfriend, "Happy anniversary.", "s1_10_boyfriend", 0.95f},
        {38.4f, S::Woman, "I love you.", "s1_11_woman", 0.93f},
        {39.6f, S::Boyfriend, "I love you more.", "s1_12_boyfriend", 0.77f},
        {45.2f, S::Woman, "So... dinner at your place tonight?", "s1_13_woman", 2.48f},
        {48.0f, S::Boyfriend, "Can't tonight, babe. Work stuff.", "s1_14_boyfriend", 2.17f},
        {50.6f, S::Woman, "Okay... another time, then.", "s1_15_woman", 2.22f},
    },
    // Scene 2: The Discovery (his house, sunset)
    {
        {7.0f, S::Woman, "He's going to love this.", "s2_01_woman", 1.84f},
        {16.5f, S::Woman, "Babe? It's me!", "s2_02_woman", 2.08f},
        {22.0f, S::Boyfriend, "Stay a little longer.", "s2_03_boyfriend", 0.95f},
        {23.4f, S::OtherMan, "She really doesn't suspect a thing?", "s2_04_otherman", 1.83f},
        {25.5f, S::Boyfriend, "Relax. She thinks I'm at work.", "s2_05_boyfriend", 2.07f},
        {27.7f, S::OtherMan, "Ha ha ha!", "s2_06_otherman", 1.41f},
        {29.3f, S::Woman, "Surprise!", "s2_07_woman", 2.80f},
        {34.4f, S::Boyfriend, "Wait, it's not what it looks like!", "s2_08_boyfriend", 1.90f},
        {38.5f, S::Boyfriend, "Please, just let me explain.", "s2_09_boyfriend", 2.11f},
    },
    // Scene 3: The Confrontation (inside, evening)
    {
        {2.8f, S::Woman, "Not what it looks like? I saw you!", "s3_01_woman", 3.46f},
        {6.5f, S::Boyfriend, "I was going to tell you...", "s3_02_boyfriend", 1.69f},
        {8.4f, S::Woman, "When? After another 'work night'?", "s3_03_woman", 3.09f},
        {12.0f, S::Woman, "Keep your jacket!", "s3_05_woman", 1.83f},
        {14.6f, S::Boyfriend, "I'm sorry. I didn't know how.", "s3_04_boyfriend", 2.27f},
        {17.3f, S::Woman, "And you! You knew about me the whole time!", "s3_07_woman", 3.17f},
        {23.5f, S::Boyfriend, "I never meant to hurt you. I swear.", "s3_08_boyfriend", 2.59f},
        {27.2f, S::Boyfriend, "Please, just listen to me.", "s3_06_boyfriend", 2.02f},
        {31.0f, S::Woman, "Don't ever call me again.", "s3_09_woman", 1.67f},
        {34.0f, S::Boyfriend, "Wait! Please, don't go!", "s3_10_boyfriend", 2.05f},
        {43.3f, S::OtherMan, "Hey... it's going to be okay.", "s3_11_otherman", 2.13f},
        {45.6f, S::Boyfriend, "Don't touch me!", "s3_12_boyfriend", 1.26f},
    },
    // Scene 4: The Journey Home: she says nothing
    {},
    // Scene 5: The Phone Theft (street, night)
    {
        {3.7f, S::Woman, "Hey! Give that back!", "s5_01_woman", 2.14f},
        {6.0f, S::Woman, "Stop! Somebody stop him!", "s5_02_woman", 3.26f},
        {16.2f, S::Woman, "No... no, no, no...", "s5_03_woman", 3.78f},
    },
    // Scene 6: The Ending (park bench, night)
    {
        {27.5f, S::Stranger, "Hey... are you okay?", "s6_01_stranger", 1.52f},
        {29.4f, S::Woman, "No... not really.", "s6_02_woman", 1.74f},
        {31.3f, S::Stranger, "Mind if I sit?", "s6_03_stranger", 0.71f},
        {35.0f, S::Stranger, "Whatever happened today... it doesn't define you.", "s6_04_stranger", 3.25f},
        {39.5f, S::Stranger, "Here. Something to smile about.", "s6_05_stranger", 2.01f},
        {42.6f, S::Woman, "Thank you.", "s6_06_woman", 0.9f},
    },
    };
    return kScenes;
}

constexpr int kTextSize = 3;
constexpr int kMaxTextWidth = 330;
constexpr int kPad = 14, kRadius = 14, kBorder = 3, kLineGap = 8;
constexpr float kLinger = 0.7f;  // seconds the bubble stays after the words end, to finish reading
const Color kInk{40, 36, 48};
const Color kPaper{255, 252, 240};

// Splits the text into lines no wider than the bubble allows
std::vector<std::string> wrap(const std::string& text) {
    std::vector<std::string> lines;
    std::string line, word;
    auto addWord = [&] {
        if (word.empty()) return;
        std::string tried = line.empty() ? word : line + " " + word;
        if (!line.empty() && textWidth(tried, kTextSize) > kMaxTextWidth) {
            lines.push_back(line);
            line = word;
        } else {
            line = tried;
        }
        word.clear();
    };
    for (char c : text) {
        if (c == ' ') addWord();
        else word += c;
    }
    addWord();
    if (!line.empty()) lines.push_back(line);
    return lines;
}

// A filled rectangle with rounded corners: two overlapping rectangles and a
// filled circle in each corner
void roundedBox(Canvas& canvas, int x, int y, int w, int h, int r, Color color) {
    fillRect(canvas, x + r, y, w - 2 * r, h, color);
    fillRect(canvas, x, y + r, w, h - 2 * r, color);
    fillCircle(canvas, x + r, y + r, r, color);
    fillCircle(canvas, x + w - r - 1, y + r, r, color);
    fillCircle(canvas, x + r, y + h - r - 1, r, color);
    fillCircle(canvas, x + w - r - 1, y + h - r - 1, r, color);
}

void drawBubble(Canvas& canvas, const Line& line, const SpeakerAt* at, const CameraView& cam, float grow) {
    const std::vector<std::string> rows = wrap(line.text);
    const int charH = 7 * kTextSize;
    int textW = 0;
    for (const std::string& r : rows) textW = std::max(textW, textWidth(r, kTextSize));
    const int fullW = textW + 2 * kPad;
    const int fullH = static_cast<int>(rows.size()) * charH + (static_cast<int>(rows.size()) - 1) * kLineGap + 2 * kPad;
    // Pops open: grows from 60% to full size
    const int w = static_cast<int>(fullW * (0.6f + 0.4f * grow)), h = static_cast<int>(fullH * (0.6f + 0.4f * grow));

    const int W = canvas.width(), H = canvas.height(), margin = 12;
    int cx = W / 2, bottom = H - margin - h;  // no known speaker: top centre
    Point tip{-1, -1};
    if (at) {
        // Where the head ends up on screen after the camera's zoom and shake
        Point head{static_cast<int>(cam.focus.x + (at->head.x - cam.focus.x) * cam.zoom) + cam.shakeX,
                   static_cast<int>(cam.focus.y + (at->head.y - cam.focus.y) * cam.zoom) + cam.shakeY};
        int r = static_cast<int>(at->radius * cam.zoom);
        tip = {std::clamp(head.x, margin, W - margin), std::min(head.y + r + 10, H - margin - h - 20)};
        cx = head.x;
        bottom = tip.y + 34;
    }
    const int x = std::clamp(cx - w / 2, margin, W - margin - w);
    const int y = std::clamp(bottom, margin, H - margin - h);

    // Tail from the box's lower edge towards the head
    const bool tail = at && tip.y < y - 6;
    const int baseX = std::clamp(tip.x, x + kRadius + 12, x + w - kRadius - 12);
    const std::vector<Point> outer = {{baseX - 14, y + 2}, {baseX + 14, y + 2}, {tip.x, tip.y}};
    const std::vector<Point> inner = {{baseX - 10, y + kBorder + 1}, {baseX + 10, y + kBorder + 1}, {tip.x, tip.y + 4}};

    roundedBox(canvas, x, y, w, h, kRadius, kInk);
    if (tail) scanlineFill(canvas, outer, kInk);
    roundedBox(canvas, x + kBorder, y + kBorder, w - 2 * kBorder, h - 2 * kBorder, kRadius - kBorder, kPaper);
    if (tail) scanlineFill(canvas, inner, kPaper);

    if (grow < 1.0f) return;
    int baseline = y + h - kPad - charH;
    for (const std::string& r : rows) {
        drawTextCentred(canvas, x + w / 2, baseline, r, kTextSize, kInk);
        baseline -= charH + kLineGap;
    }
}

}  // namespace

const std::vector<Line>& sceneLines(int scene) {
    static const std::vector<Line> none;
    return scene >= 1 && scene <= 6 ? allScenes()[scene - 1] : none;
}

std::vector<Span> talkSpans(const std::vector<Line>& lines, Speaker who) {
    std::vector<Span> spans;
    for (const Line& l : lines)
        if (l.who == who) spans.push_back({l.time, l.time + l.length});
    return spans;
}

void drawSpeechBubbles(Canvas& canvas, const std::vector<Line>& lines, float t, const std::vector<SpeakerAt>& speakers,
                       const CameraView& camera) {
    for (size_t i = 0; i < lines.size(); ++i) {
        const Line& l = lines[i];
        // Stays up a moment after the words, but goes when the next line starts
        float until = l.time + l.length + kLinger;
        if (i + 1 < lines.size()) until = std::min(until, std::max(lines[i + 1].time - 0.05f, l.time + l.length));
        if (t < l.time - 0.05f || t >= until) continue;

        const SpeakerAt* at = nullptr;
        for (const SpeakerAt& s : speakers)
            if (s.who == l.who) at = &s;
        drawBubble(canvas, l, at, camera, std::clamp((t - l.time + 0.05f) / 0.12f, 0.0f, 1.0f));
    }
}
