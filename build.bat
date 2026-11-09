@echo off
rem Builds and runs the project with g++ (MinGW / MSYS2) from cmd or PowerShell.
if not exist build mkdir build
if not exist output mkdir output
setlocal enabledelayedexpansion
set SOURCES=
for /r src %%f in (*.cpp) do set SOURCES=!SOURCES! "%%f"
g++ -std=c++17 -O2 -Wall -Wextra -static %SOURCES% -o build\behind_the_smile.exe || exit /b 1
build\behind_the_smile.exe
