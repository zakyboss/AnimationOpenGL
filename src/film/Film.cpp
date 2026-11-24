#include "Film.h"
#include <algorithm>
#include <filesystem>
#include "Dialogue.h"
#include "Effects.h"
#include "Scenes.h"

namespace {

struct SceneEntry {
    float length;   // seconds
    bool fadeOut;   // fade to black at the end (the ending handles its own)
    void (*play)(Canvas&, float);
};

// The film in order. Lengths add up to 5:00.
const SceneEntry kScenes[] = {
    {60.0f, true, playScene1},   // 0:00 - 1:00  The Relationship   (park, afternoon)
    {45.0f, true, playScene2},   // 1:00 - 1:45  The Discovery      (his house, sunset)
    {55.0f, true, playScene3},   // 1:45 - 2:40  The Confrontation  (inside, evening)
    {40.0f, true, playScene4},   // 2:40 - 3:20  The Journey Home   (street, night)
    {45.0f, true, playScene5},   // 3:20 - 4:05  The Phone Theft    (street, night)
    {55.0f, false, playScene6},  // 4:05 - 5:00  The Ending         (park bench, night)
};

constexpr float kFadeTime = 1.0f;

}  // namespace

std::vector<SoundCue> filmSound() {
    const std::string res = "Resources/";
    const std::vector<float> at = filmChapters();  // where each scene starts on the film's timeline
    std::vector<SoundCue> cues;
    auto add = [&](const SoundCue& c) { cues.push_back(c); };

    // ---- Music ----
    // The couple walk into the park to the song; as they leave it hands over
    // to the softer "Blue Ribbons", which cuts out the moment she sees them
    add(music(res + "lauv-steal-the-show-from-elemental_80HrtbSg.mp3", 1.0f, 52.0f, 0, 0.5f, 2.5f).from(0.65f).normalised(0.42f));
    add(music(res + "Background- Blue Ribbons - TrackTribe.mp3", 50.0f, at[1] + 30.3f, 0, 3.0f, 0.3f).normalised(0.26f));
    // Low and heavy from when she breaks down, fading before the thief strikes
    add(music(res + "Background-Save me - Patrick Patrikios.mp3", at[2] + 22.0f, at[4] - 14.0f, 0, 4.0f, 5.0f).normalised(0.26f));
    // After the theft, alone, and on the bench to the end
    add(music(res + "Sad-Never Play - Jeremy Blake.mp3", at[4] + 14.0f, 300.0f, 0, 4.0f, 5.0f).normalised(0.3f));

    // ---- Voices: every line in the dialogue tables that has a recording ----
    for (int scene = 1; scene <= 6; ++scene)
        for (const Line& line : sceneLines(scene)) {
            const std::string file = res + line.clip + ".mp3";
            if (!line.clip.empty() && std::filesystem::exists(file)) add(voice(file, at[scene - 1] + line.time));
        }

    // ---- Background sound ----
    const std::string park = res + "Park_in_the_afternoo_#1-1794921274102.mp3";
    const std::string fountain = res + "Small_fountain_trick_#3-1794921350777.mp3";
    const std::string crickets = res + "Quiet_suburban_eveni_#3-1794921586541.mp3";
    const std::string street = res + "City_street_at_night_#1-1794921996641.mp3";
    add(ambience(park, 0.0f, at[1], 0.22f));
    add(ambience(fountain, 0.0f, at[1], 0.10f).pan(0.55f));  // the fountain is on the right
    add(ambience(crickets, at[1], at[1] + 20.5f, 0.22f).fades(1.5f, 1.5f));
    add(ambience(crickets, at[1] + 21.0f, at[3], 0.06f).fades(1.0f, 1.0f));  // faintly through the window, indoors
    add(ambience(street, at[3], at[5], 0.2f));
    add(ambience(crickets, at[5], 298.0f, 0.2f));
    add(ambience(fountain, at[5], 298.0f, 0.07f).pan(0.55f));

    // ---- Effects ----
    // The crying recording is one long sob; it is repeated, softer each time
    const std::string sob = res + "woman_sob_long.mp3";
    auto sobbing = [&](float t, float level) {
        add(effect(sob, t).from(1.0f).until(t + 2.8f).normalised(level).fades(0.05f, 0.5f));
    };

    // Scene 1: the park
    const std::string splash = res + "Small_bird_splashing_#2-1794921389787.mp3";
    for (float t = 2.2f + 0.35f; t < 58.0f; t += 4.4f)  // every other dip of the bird's head
        add(effect(splash, t).from(1.0f).until(t + 1.2f).normalised(0.12f).fades(0.05f, 0.4f).pan(0.6f));
    add(effect(res + "Strong_gust_of_cold__#4-1794921430743.mp3", 12.0f).from(4.0f).until(16.8f).normalised(0.45f)
            .fades(0.4f, 1.5f).sweep(-0.6f, 0.6f));
    const std::string jacket = res + "Jacket_being_taken_o_#1-1794921474720.mp3";
    add(effect(jacket, 14.0f).from(0.2f).until(16.0f).normalised(0.35f).fades(0.05f, 0.4f));
    add(effect(jacket, 17.6f).from(1.6f).until(19.0f).normalised(0.3f).fades(0.05f, 0.4f));
    add(effect(res + "Soft_magical_chime,__#1-1794921552157.mp3", 37.0f).from(0.1f).normalised(0.3f));

    // Scene 2: the discovery
    const std::string creak = res + "Front_door_creaking__#1-1794921651608.mp3";
    add(effect(res + "Three_knocks_on_a_wo_#1-1794921623860.mp3", at[1] + 15.1f).from(1.55f).until(at[1] + 16.6f)
            .normalised(0.55f));
    add(effect(creak, at[1] + 17.4f).until(at[1] + 19.6f).normalised(0.4f).fades(0.05f, 0.6f));
    add(effect(creak, at[1] + 28.9f).from(1.2f).until(at[1] + 30.0f).normalised(0.35f).fades(0.02f, 0.3f).pan(0.6f));
    add(effect("synth:sting", at[1] + 30.4f, 0.8f));
    add(effect("synth:thud", at[1] + 32.15f, 0.7f).pan(0.6f));  // the gift hits the floor
    sobbing(at[1] + 36.6f, 0.35f);
    sobbing(at[1] + 41.0f, 0.3f);

    // Scene 3: the confrontation
    add(effect("synth:whoosh", at[2] + 12.8f, 0.5f).sweep(0.4f, -0.1f));  // the jacket thrown
    add(effect(jacket, at[2] + 13.55f).from(1.8f).until(at[2] + 14.6f).normalised(0.3f).fades(0.02f, 0.3f));
    add(effect("synth:thud", at[2] + 13.6f, 0.25f));
    sobbing(at[2] + 22.4f, 0.5f);
    sobbing(at[2] + 25.0f, 0.4f);
    sobbing(at[2] + 27.6f, 0.45f);
    add(effect("synth:crunch", at[2] + 36.6f, 0.8f).pan(0.65f));  // she stamps on the gift
    add(effect(res + "Front_door_slammed_h_#2-1794921915537.mp3", at[2] + 38.2f).from(0.95f).until(at[2] + 39.4f)
            .normalised(0.8f).pan(0.6f));
    add(effect("synth:shove", at[2] + 45.6f, 0.7f).pan(0.2f));

    // Scene 4: the walk home
    const std::string sniff = res + "woman_sniff_#4-1794921178481.mp3";
    for (float t : {13.0f, 17.5f, 29.0f})
        add(effect(sniff, at[3] + t).from(0.1f).until(at[3] + t + 0.75f).normalised(0.4f).fades(0.02f, 0.2f));
    const std::string car = res + "Car_driving_past_fro_#1-1794922034398.mp3";
    add(effect(car, at[3] + 22.0f).until(at[3] + 26.0f).normalised(0.45f).sweep(-1.0f, 1.0f).fades(0.3f, 0.5f));

    // Scene 5: the phone theft
    const std::string running = res + "Two_people_running_o_#1-1794922088648.mp3";
    add(effect("synth:whoosh", at[4] + 3.3f, 0.6f));  // the snatch
    add(effect(running, at[4] + 5.4f).until(at[4] + 7.8f).normalised(0.4f).sweep(0.0f, 1.0f).fades(0.1f, 0.6f));
    add(effect(running, at[4] + 10.0f).until(at[4] + 14.0f).normalised(0.4f).sweep(-1.0f, 0.0f).fades(0.3f, 1.2f));
    add(effect(res + "woman_panting_#1-1794921231936.mp3", at[4] + 14.3f).from(0.05f).until(at[4] + 18.5f)
            .normalised(0.35f).fades(0.1f, 1.0f));
    sobbing(at[4] + 20.0f, 0.5f);
    sobbing(at[4] + 22.6f, 0.45f);
    sobbing(at[4] + 25.2f, 0.4f);
    add(effect(car, at[4] + 23.0f).until(at[4] + 27.0f).normalised(0.45f).sweep(-1.0f, 1.0f).fades(0.3f, 0.5f));

    // Scene 6: the bench
    for (float t : {17.0f, 19.6f, 22.2f, 24.8f}) sobbing(at[5] + t, t < 24.0f ? 0.45f : 0.35f);
    sobbing(at[5] + 34.0f, 0.3f);
    sobbing(at[5] + 38.4f, 0.25f);
    return cues;
}

float filmDuration() {
    float total = 0;
    for (const SceneEntry& s : kScenes) total += s.length;
    return total;
}

std::vector<float> filmChapters() {
    std::vector<float> starts;
    float start = 0;
    for (const SceneEntry& s : kScenes) {
        starts.push_back(start);
        start += s.length;
    }
    return starts;
}

void renderFilm(Canvas& canvas, float t) {
    // Find the scene playing at time t
    float start = 0;
    const SceneEntry* scene = &kScenes[0];
    for (const SceneEntry& s : kScenes) {
        scene = &s;
        if (t < start + s.length) break;
        start += s.length;
    }
    if (t >= filmDuration()) start = filmDuration() - scene->length;  // hold the last frame

    const float local = std::min(t - start, scene->length);
    scene->play(canvas, local);

    // Fade in from black at the start of each scene, and out at the end
    float fade = 1.0f - local / kFadeTime;
    if (scene->fadeOut) fade = std::max(fade, 1.0f - (scene->length - local) / kFadeTime);
    fadeToColor(canvas, Palette::Black, std::clamp(fade, 0.0f, 1.0f));
}
