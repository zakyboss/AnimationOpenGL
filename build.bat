@echo off
rem Builds and runs the project with g++ (MSYS2) from cmd or PowerShell.
rem Arguments are passed to the program: build.bat --stills, build.bat --frame 12.5
if not exist build mkdir build
if not exist output mkdir output
setlocal enabledelayedexpansion
set SOURCES=
for /r src %%f in (*.cpp) do set SOURCES=!SOURCES! "%%f"
g++ -std=c++17 -O2 -Wall -Wextra -static -DFREEGLUT_STATIC %SOURCES% -o build\behind_the_smile.exe -lfreeglut -lopengl32 -lglu32 -lwinmm -lgdi32 -lmfplat -lmfreadwrite -lmfuuid -lole32 || exit /b 1
build\behind_the_smile.exe %*
