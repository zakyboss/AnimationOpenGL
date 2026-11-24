// Behind the Smile: a 2D animated short film in C++ and OpenGL.
//
//   behind_the_smile                 play the film in a window
//   behind_the_smile --stills        write the test sheets and scene stills to output/
//   behind_the_smile --frame T [F]   write the film frame at T seconds to output/frame.bmp (or F)
//   behind_the_smile --audio [F]     mix the whole soundtrack to output/soundtrack.wav (or F)
#include <filesystem>
#include <iostream>
#include <string>
#include "app/Player.h"
#include "film/Film.h"
#include "film/Stills.h"

int main(int argc, char** argv) {
    const std::string mode = argc > 1 ? argv[1] : "";

    if (mode == "--stills") {
        renderStills();
        return 0;
    }

    if (mode == "--frame" && argc > 2) {
        std::filesystem::create_directories("output");
        const std::string path = argc > 3 ? argv[3] : "output/frame.bmp";
        Canvas canvas(kFilmWidth, kFilmHeight);
        renderFilm(canvas, std::stof(argv[2]));
        if (!canvas.saveBMP(path)) {
            std::cerr << "Failed to save " << path << "\n";
            return 1;
        }
        std::cout << "Saved " << path << "\n";
        return 0;
    }

    if (mode == "--audio") {
        std::filesystem::create_directories("output");
        const std::string path = argc > 2 ? argv[2] : "output/soundtrack.wav";
        if (!Soundtrack::exportWav(filmSound(), filmDuration(), path)) {
            std::cerr << "Failed to save " << path << "\n";
            return 1;
        }
        std::cout << "Saved " << path << "\n";
        return 0;
    }

    Canvas canvas(kFilmWidth, kFilmHeight);
    runPlayer(argc, argv, canvas, filmDuration(), kFilmFps, filmChapters(), filmSound(), renderFilm);
    return 0;
}
