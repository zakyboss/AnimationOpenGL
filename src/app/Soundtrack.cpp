#include "Soundtrack.h"
#include <windows.h>
#include <mmsystem.h>
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <map>
#include "Synth.h"

namespace {

constexpr int kRate = 44100;        // samples per second, per channel
constexpr int kBufferFrames = 1024;  // about 23 ms per buffer
constexpr int kBuffers = 4;
// How far ahead of the speakers the mixer works: the buffers already queued
constexpr double kLatency = (kBuffers - 1) * kBufferFrames / double(kRate);

// Decodes up to maxSeconds of a sound file into 16-bit stereo at kRate, with
// Media Foundation's source reader (it handles MP3, the AAC in MP4, WAV...).
bool decodeFile(const std::string& path, float maxSeconds, std::vector<short>& out, const std::atomic<bool>& quit) {
    wchar_t wide[MAX_PATH], full[MAX_PATH];
    MultiByteToWideChar(CP_UTF8, 0, path.c_str(), -1, wide, MAX_PATH);
    GetFullPathNameW(wide, MAX_PATH, full, nullptr);

    IMFSourceReader* reader = nullptr;
    if (FAILED(MFCreateSourceReaderFromURL(full, nullptr, &reader))) return false;
    reader->SetStreamSelection(MF_SOURCE_READER_ALL_STREAMS, FALSE);
    reader->SetStreamSelection(MF_SOURCE_READER_FIRST_AUDIO_STREAM, TRUE);

    IMFMediaType* type = nullptr;
    MFCreateMediaType(&type);
    type->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
    type->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_PCM);
    type->SetUINT32(MF_MT_AUDIO_NUM_CHANNELS, 2);
    type->SetUINT32(MF_MT_AUDIO_SAMPLES_PER_SECOND, kRate);
    type->SetUINT32(MF_MT_AUDIO_BITS_PER_SAMPLE, 16);
    type->SetUINT32(MF_MT_AUDIO_BLOCK_ALIGNMENT, 4);
    type->SetUINT32(MF_MT_AUDIO_AVG_BYTES_PER_SECOND, kRate * 4);
    HRESULT ok = reader->SetCurrentMediaType(MF_SOURCE_READER_FIRST_AUDIO_STREAM, nullptr, type);
    type->Release();
    if (FAILED(ok)) {
        reader->Release();
        return false;
    }

    const size_t limit = static_cast<size_t>(maxSeconds * kRate) * 2;
    while (out.size() < limit && !quit) {
        DWORD flags = 0;
        IMFSample* sample = nullptr;
        if (FAILED(reader->ReadSample(MF_SOURCE_READER_FIRST_AUDIO_STREAM, 0, nullptr, &flags, nullptr, &sample))) break;
        if (sample) {
            IMFMediaBuffer* buffer = nullptr;
            sample->ConvertToContiguousBuffer(&buffer);
            BYTE* data = nullptr;
            DWORD bytes = 0;
            buffer->Lock(&data, nullptr, &bytes);
            const short* s = reinterpret_cast<const short*>(data);
            out.insert(out.end(), s, s + bytes / sizeof(short));
            buffer->Unlock();
            buffer->Release();
            sample->Release();
        }
        if (flags & MF_SOURCE_READERF_ENDOFSTREAM) break;
    }
    reader->Release();
    if (out.size() > limit) out.resize(limit);
    return !out.empty();
}

}  // namespace

struct Soundtrack::Clip {
    std::string file;
    float maxSeconds = 0;       // only as much as the cues need is decoded
    float firstNeeded = 1e9f;   // earliest cue start, to decide the loading order
    std::vector<short> samples;  // stereo, interleaved
    size_t frames = 0;
    float peak = 1.0f;          // loudest sample, 0 to 1
    float lead = 0.0f;          // seconds of near-silence at the start
    std::atomic<bool> ready{false};
};

SoundCue music(const std::string& file, float start, float end, float volume, float fadeIn, float fadeOut) {
    SoundCue c;
    c.file = file;
    c.start = start;
    c.end = end;
    c.volume = volume;
    c.fadeIn = fadeIn;
    c.fadeOut = fadeOut;
    c.bus = Bus::Music;
    return c;
}

SoundCue voice(const std::string& file, float start) {
    SoundCue c;
    c.file = file;
    c.start = start;
    c.bus = Bus::Voice;
    c.normalise = true;
    c.volume = 0.85f;
    c.trim = true;
    c.fadeOut = 0.05f;
    return c;
}

SoundCue effect(const std::string& file, float start, float volume) {
    SoundCue c;
    c.file = file;
    c.start = start;
    c.volume = volume;
    c.fadeOut = 0.05f;
    return c;
}

SoundCue ambience(const std::string& file, float start, float end, float level) {
    SoundCue c;
    c.file = file;
    c.start = start;
    c.end = end;
    c.loop = true;
    c.normalise = true;
    c.volume = level;
    c.fadeIn = 2.0f;
    c.fadeOut = 2.0f;
    return c;
}

Soundtrack::Soundtrack(const std::vector<SoundCue>& cues, bool live) {
    // One clip per file, decoded only as far as the cues reach into it
    std::map<std::string, std::shared_ptr<Clip>> byFile;
    for (const SoundCue& c : cues) {
        std::shared_ptr<Clip>& clip = byFile[c.file];
        if (!clip) {
            clip = std::make_shared<Clip>();
            clip->file = c.file;
            clips_.push_back(clip);
        }
        float need = (c.loop || c.end <= 0) ? 1e6f : c.offset + (c.end - c.start) + 2.0f;  // +2 s covers the trimmed lead
        clip->maxSeconds = std::max(clip->maxSeconds, need);
        clip->firstNeeded = std::min(clip->firstNeeded, c.start);
        cues_.push_back({c, clip});
    }
    std::stable_sort(clips_.begin(), clips_.end(),
                     [](const auto& a, const auto& b) { return a->firstNeeded < b->firstNeeded; });

    if (!live) return;
    loader_ = std::thread(&Soundtrack::loaderThread, this);
    mixer_ = std::thread(&Soundtrack::mixerThread, this);
}

bool Soundtrack::exportWav(const std::vector<SoundCue>& cues, float duration, const std::string& path) {
    Soundtrack track(cues, false);
    track.loaderThread();  // load everything first, on this thread

    const uint32_t frames = static_cast<uint32_t>(duration * kRate);
    std::vector<short> pcm(static_cast<size_t>(frames) * 2);
    for (uint32_t done = 0; done < frames; done += kBufferFrames) {
        int n = static_cast<int>(std::min<uint32_t>(kBufferFrames, frames - done));
        track.mix(&pcm[static_cast<size_t>(done) * 2], n, done / double(kRate));
    }

    FILE* f = std::fopen(path.c_str(), "wb");
    if (!f) return false;
    auto u32 = [&](uint32_t v) { std::fwrite(&v, 4, 1, f); };
    auto u16 = [&](uint16_t v) { std::fwrite(&v, 2, 1, f); };
    const uint32_t bytes = frames * 4;
    std::fwrite("RIFF", 1, 4, f); u32(36 + bytes); std::fwrite("WAVEfmt ", 1, 8, f);
    u32(16); u16(1); u16(2); u32(kRate); u32(kRate * 4); u16(4); u16(16);
    std::fwrite("data", 1, 4, f); u32(bytes);
    std::fwrite(pcm.data(), 2, pcm.size(), f);
    std::fclose(f);
    return true;
}

Soundtrack::~Soundtrack() {
    quit_ = true;
    if (loader_.joinable()) loader_.join();
    if (mixer_.joinable()) mixer_.join();
}

void Soundtrack::update(float filmTime, bool playing) {
    std::lock_guard<std::mutex> lock(mutex_);
    filmTime_ = filmTime;
    if (playing && !playing_) resync_ = true;
    playing_ = playing;
}

void Soundtrack::restart() {
    std::lock_guard<std::mutex> lock(mutex_);
    resync_ = true;
}

void Soundtrack::loaderThread() {
    CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    MFStartup(MF_VERSION);
    for (const std::shared_ptr<Clip>& clip : clips_) {
        if (quit_) break;
        const std::string synthPrefix = "synth:";
        bool ok = clip->file.compare(0, synthPrefix.size(), synthPrefix) == 0
                      ? synthesize(clip->file.substr(synthPrefix.size()), kRate, clip->samples)
                      : decodeFile(clip->file, clip->maxSeconds, clip->samples, quit_);
        if (!ok) {
            std::fprintf(stderr, "Could not load sound %s\n", clip->file.c_str());
            continue;
        }
        clip->frames = clip->samples.size() / 2;

        int peak = 1;
        for (short s : clip->samples) peak = std::max(peak, std::abs(static_cast<int>(s)));
        clip->peak = peak / 32768.0f;
        // The sound "starts" at the first sample above a tenth of the peak; keep 30 ms before it
        size_t first = 0;
        while (first < clip->samples.size() && std::abs(static_cast<int>(clip->samples[first])) <= peak / 10) ++first;
        clip->lead = std::max(0.0f, (first / 2) / float(kRate) - 0.03f);
        clip->ready = true;
    }
    MFShutdown();
    CoUninitialize();
}

void Soundtrack::mix(short* out, int frames, double t0) {
    static std::vector<float> left, right, duck;
    left.assign(frames, 0.0f);
    right.assign(frames, 0.0f);
    duck.resize(frames);
    const double t1 = t0 + frames / double(kRate);

    // Is anyone speaking in (or just around) this buffer? Then the music dips.
    bool speaking = false;
    for (const Cue& c : cues_) {
        if (c.cue.bus != Bus::Voice || !c.clip->ready) continue;
        double length = c.clip->frames / double(kRate) - (c.cue.trim ? c.clip->lead : 0) - c.cue.offset;
        if (t1 > c.cue.start - 0.15 && t0 < c.cue.start + length) speaking = true;
    }
    const float duckTarget = speaking ? 0.4f : 1.0f, duckStep = 1.0f / (0.3f * kRate);
    for (int i = 0; i < frames; ++i) {
        duck_ += std::clamp(duckTarget - duck_, -duckStep, duckStep);
        duck[i] = duck_;
    }

    for (const Cue& c : cues_) {
        const SoundCue& q = c.cue;
        const Clip& clip = *c.clip;
        if (!clip.ready || clip.frames == 0) continue;

        const size_t first = static_cast<size_t>(((q.trim ? clip.lead : 0.0f) + q.offset) * kRate);
        if (first >= clip.frames) continue;
        const size_t span = clip.frames - first;  // frames available from the start point
        const double stop = q.end > 0 ? q.end : q.start + span / double(kRate);
        if (t1 <= q.start || t0 >= stop) continue;

        const float base = q.normalise ? q.volume / clip.peak : q.volume;
        const float length = static_cast<float>(stop - q.start);
        for (int i = 0; i < frames; ++i) {
            const double t = t0 + i / double(kRate);
            if (t < q.start || t >= stop) continue;
            const float local = static_cast<float>(t - q.start);
            size_t n = static_cast<size_t>(local * kRate);
            if (q.loop) n %= span;
            else if (n >= span) break;
            const size_t at = 2 * (first + n);

            float env = 1.0f;
            if (q.fadeIn > 0) env = std::min(env, local / q.fadeIn);
            if (q.fadeOut > 0) env = std::min(env, static_cast<float>(stop - t) / q.fadeOut);
            const float pan = q.panFrom + (q.panTo - q.panFrom) * (local / length);
            float g = base * env * (q.bus == Bus::Music ? duck[i] : 1.0f) / 32768.0f;
            left[i] += clip.samples[at] * g * std::min(1.0f, 1.0f - pan);
            right[i] += clip.samples[at + 1] * g * std::min(1.0f, 1.0f + pan);
        }
    }

    // Gentle limiter (tanh) so loud moments round off instead of clipping
    const bool silent = muted_;
    for (int i = 0; i < frames; ++i) {
        out[2 * i] = silent ? 0 : static_cast<short>(std::tanh(left[i]) * 32767.0f);
        out[2 * i + 1] = silent ? 0 : static_cast<short>(std::tanh(right[i]) * 32767.0f);
    }
}

void Soundtrack::mixerThread() {
    WAVEFORMATEX format{};
    format.wFormatTag = WAVE_FORMAT_PCM;
    format.nChannels = 2;
    format.nSamplesPerSec = kRate;
    format.wBitsPerSample = 16;
    format.nBlockAlign = 4;
    format.nAvgBytesPerSec = kRate * 4;

    HANDLE done = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    HWAVEOUT device = nullptr;
    if (waveOutOpen(&device, WAVE_MAPPER, &format, reinterpret_cast<DWORD_PTR>(done), 0, CALLBACK_EVENT) !=
        MMSYSERR_NOERROR) {
        std::fprintf(stderr, "No sound device: the film will play silently\n");
        CloseHandle(done);
        return;
    }

    std::vector<short> memory(kBuffers * kBufferFrames * 2);
    WAVEHDR headers[kBuffers] = {};
    for (int b = 0; b < kBuffers; ++b) {
        headers[b].lpData = reinterpret_cast<LPSTR>(&memory[b * kBufferFrames * 2]);
        headers[b].dwBufferLength = kBufferFrames * 4;
        waveOutPrepareHeader(device, &headers[b], sizeof(WAVEHDR));
        headers[b].dwFlags |= WHDR_DONE;  // empty, ready to fill
    }

    while (!quit_) {
        for (WAVEHDR& h : headers) {
            if (!(h.dwFlags & WHDR_DONE)) continue;
            short* out = reinterpret_cast<short*>(h.lpData);

            double start = 0;
            bool playing;
            {
                std::lock_guard<std::mutex> lock(mutex_);
                playing = playing_;
                // Follow the picture: jump when told to, or if we have drifted apart
                const double target = filmTime_ + kLatency;
                if (playing && (resync_ || std::fabs(audioTime_ - target) > 0.25)) {
                    audioTime_ = target;
                    resync_ = false;
                }
                start = audioTime_;
                if (playing) audioTime_ += kBufferFrames / double(kRate);
            }
            if (playing) mix(out, kBufferFrames, start);
            else std::fill(out, out + kBufferFrames * 2, 0);

            h.dwFlags &= ~WHDR_DONE;
            waveOutWrite(device, &h, sizeof(WAVEHDR));
        }
        WaitForSingleObject(done, 20);
    }

    waveOutReset(device);
    for (WAVEHDR& h : headers) waveOutUnprepareHeader(device, &h, sizeof(WAVEHDR));
    waveOutClose(device);
    CloseHandle(done);
}
