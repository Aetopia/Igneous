@echo off
cd "%~dp0/src"

rd /q /s "bin"
md "bin"

rd /q /s "obj"
md "obj"

windres.exe -i "Resources\Application.rc" -o "obj\Application.o"
gcc.exe -Oz -s -Wl,--gc-sections,--exclude-all-symbols,--wrap=memcpy,--wrap=memset -nostdlib -static -shared -municode -e DllMain "Library.c" "obj\Application.o" -lminhook -lkernel32 -luser32 -lshlwapi -ldxgi -o "bin\vcruntime140_1.dll"