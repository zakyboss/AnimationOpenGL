#pragma once
#include <atomic>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

// Which group a sound belongs to. Music is turned down while anyone speaks.
enum class Bus { Music, Voice, Effect };

// One sound placed on the film's timeline. Build one with music(), voice(),
// effect() or ambience(), then adjust it with the chained setters, e.g.
//   effect("Resources/knock.mp3", 75.1f).from(1.55f).gain(0.6f)
struct SoundCue {
    std::string file;        // an .mp3/.mp4/.wav in Resources/, or "synth:<name>" for a sound made in code
    float start = 0;         // film time in seconds
    float end = 0;           // film time to stop; 0 plays the clip once to its end
    float volume = 1;        // loudness: a gain, or the peak to reach when normalised
    float fadeIn = 0, fadeOut = 0;
    float offset = 0;        // seconds into the file to begin (to skip silence)
    float panFrom = 0, panTo = 0;  // -1 left ... 1 right, moving from one to the other over the cue
    Bus bus = Bus::Effect;
    bool loop = false;       // repeat the clip until end
    bool normalise = false;  // scale the clip so its loudest sample reaches volume
    bool trim = false;       // skip the silence at the start of the file automatically

    SoundCue& from(float seconds) { offset = seconds; return *this; }
    SoundCue& until(float filmTime) { end = filmTime; return *this; }
    SoundCue& gain(float v) { volume = v; return *this; }
    SoundCue& fades(float in, float out) { fadeIn = in; fadeOut = out; return *this; }
    SoundCue& pan(float p) { panFrom = panTo = p; return *this; }
    SoundCue& sweep(float fromPan, float toPan) { panFrom = fromPan; panTo = toPan; return *this; }
    SoundCue& normalised(float peak) { normalise = true; volume = peak; return *this; }
};

// Background music from start to end.
SoundCue music(const std::string& file, float start, float end, float volume, float fadeIn, float fadeOut);
// A spoken line: silence trimmed, levelled with the other lines, music ducks under it.
SoundCue voice(const std::string& file, float start);
// A one-off sound effect at its natural loudness.
SoundCue effect(const std::string& file, float start, float volume = 1.0f);
// A looping background sound (birds, traffic, crickets), levelled and faded in and out.
SoundCue ambience(const std::string& file, float start, float end, float level);

// Plays the film's sound in sync with the picture. Every file is decoded once
// with Windows Media Foundation (on a loader thread, in the order the film
// needs them), and all cues are mixed sample by sample in our own code and
// sent to the sound card with waveOut. Since the mix at any moment is worked
// out from the film time alone, seeking and pausing are instant.
class Soundtrack {
public:
    explicit Soundtrack(const std::vector<SoundCue>& cues, bool live = true);
    ~Soundtrack();

    // Mixes the whole soundtrack offline into a 16-bit stereo WAV file.
    static bool exportWav(const std::vector<SoundCue>& cues, float duration, const std::string& path);

    // Call every tick with the film time and whether it is playing.
    void update(float filmTime, bool playing);
    // Call after a jump, so the sound follows the picture at once.
    void restart();
    void setMuted(bool muted) { muted_ = muted; }
    bool muted() const { return muted_; }

    struct Clip;  // decoded audio, shared by every cue that uses the file

private:
    struct Cue {
        SoundCue cue;
        std::shared_ptr<Clip> clip;
    };

    void loaderThread();
    void mixerThread();
    void mix(short* out, int frames, double startTime);

    std::vector<Cue> cues_;
    std::vector<std::shared_ptr<Clip>> clips_;  // in the order they are first needed

    std::mutex mutex_;
    double filmTime_ = 0;
    bool playing_ = false, resync_ = true;
    std::atomic<bool> muted_{false}, quit_{false};
    double audioTime_ = 0;  // film time of the next sample the mixer writes
    float duck_ = 1.0f;     // current music level under speech

    std::thread loader_, mixer_;
};
