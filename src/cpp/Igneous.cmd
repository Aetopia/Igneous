@echo off
cd "%~dp0"

rd /q /s "bin"
rd /q /s "obj"

md "bin"
md "obj"

gcc.exe -Oz -s -Wl,--gc-sections,--exclude-all-symbols -shared -e "" "Library.c" "Minecraft.c" "Game.c" "App.c" -lkernel32 -lole32 -luser32 -o "bin\Igneous.dll"

upx --compress-exports=0 --ultra-brute "bin\*"