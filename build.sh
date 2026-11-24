#!/bin/sh
# Builds and runs the project with g++ (Git Bash or MSYS2 on Windows).
# Arguments are passed to the program: ./build.sh --stills, ./build.sh --frame 12.5
set -e
mkdir -p build output
g++ -std=c++17 -O2 -Wall -Wextra -static -DFREEGLUT_STATIC $(find src -name '*.cpp') -o build/behind_the_smile \
    -lfreeglut -lopengl32 -lglu32 -lwinmm -lgdi32 -lmfplat -lmfreadwrite -lmfuuid -lole32
./build/behind_the_smile "$@"
