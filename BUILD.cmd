@echo off

cd "%~dp0"
rd /q /s "src/bin"

set "CC=x86_64-w64-mingw32-gcc"
set "CMAKE_GENERATOR=MinGW Makefiles"

cmake.exe -S "." -B "src/obj" --fresh
cmake.exe --build "src/obj" --clean-first