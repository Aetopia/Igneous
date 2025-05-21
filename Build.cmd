@echo off
cd "%~dp0/src"

rd /q /s "bin"
rd /q /s "obj"

md "bin"
md "obj"

windres.exe -i "Resources\Library.rc" -o "obj\Library.o"

gcc.exe -Oz -nostdlib -s -Wl,--gc-sections,--exclude-all-symbols -shared -e "" "obj\Library.o" "Minecraft.c" "Library.c" -lkernel32 -lole32 -luser32 -o "bin\Igneous.dll"

upx --compress-exports=0 --ultra-brute "bin\*"