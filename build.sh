#!/bin/sh
# Builds and runs the project with g++ (Git Bash, MSYS2, Linux or macOS).
set -e
mkdir -p build output
g++ -std=c++17 -O2 -Wall -Wextra -static $(find src -name '*.cpp') -o build/behind_the_smile
./build/behind_the_smile
