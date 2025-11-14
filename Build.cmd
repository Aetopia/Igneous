@echo off
cd "%~dp0/src"

rd /q /s "bin"
md "bin"

rd /q /s "obj"
md "obj"

windres.exe -i "Resources\Application.rc" -o "obj\Application.o"
gcc.exe -Wl,--gc-sections,--exclude-all-symbols,--wrap=memcpy,--wrap=memset -Oz -nostdlib -static -shared -s -e DllMain "Library.c" "obj\Application.o" -lMinHook -lKernel32 -lUser32 -lDXGI -o "bin\vcruntime140_1.dll"