cmake_minimum_required(VERSION 3.10)
project("" LANGUAGES C)

set(CMAKE_STANDARD_LIBRARIES "")
set(CMAKE_EXE_LINKER_FLAGS_INIT "")
set(CMAKE_SHARED_LIBRARY_PREFIX "")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/src/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/src/obj")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/src/obj")

include(FetchContent)
FetchContent_Declare(minhook GIT_REPOSITORY https://github.com/TsudaKageyu/minhook.git GIT_TAG v1.3.4)
FetchContent_MakeAvailable(minhook)

add_compile_options(-O3)
add_compile_definitions(UNICODE INITGUID WIN32_LEAN_AND_MEAN)
add_link_options(-Wl,--gc-sections,--exclude-all-symbols,--wrap=memcpy,--wrap=memset -nostdlib -s)

add_library(vcruntime140_1 SHARED "src/Library.c" "src/Resources/Library.rc")

target_link_options(vcruntime140_1 PRIVATE -e DllMain)
target_link_libraries(vcruntime140_1 PRIVATE minhook kernel32 dwmapi user32 dxgi gdi32)