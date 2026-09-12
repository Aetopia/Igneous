# Igneous

> [!CAUTION]
> **Only supported on Windows x64!**

A runtime patch for Minecraft: Bedrock Edition (GDK) that fixes bugs.

## Fixes

<div align="center">

|Issue|Summary|
|:-:|:-:|
|[MCPE-15796](https://bugs.mojang.com/browse/MCPE-15796)|Cursor is not recentered upon the opening of a new gui|
|[MCPE-98861](https://bugs.mojang.com/browse/MCPE-98861)|Significant input delay on devices with Render Dragon|
|[MCPE-110006](https://bugs.mojang.com/browse/MCPE-110006)|Vsync not being able to be turned off|
|[MCPE-166745](https://bugs.mojang.com/browse/MCPE-166745)|FPS is capped at double the screen's refresh rate when v-sync is disabled|
|[MCPE-230832](https://bugs.mojang.com/browse/MCPE-230832)|Mouse cursor moves in the background during gameplay, disrupting gameplay|

</div>

## Usage
- [Download](https://github.com/Aetopia/Igneous/releases/latest/download/vcruntime140_1.dll) the latest release of Igneous.

- Run the following command in PowerShell to find where the game is located:

    ```powershell
    & "$ENV:SystemRoot\explorer.exe" "$((Get-AppxPackage "Microsoft.MinecraftUWP").InstallLocation)"
    ```

- Place the dynamic link library in the opened folder & launch the game.

## FAQ

#### How can I disable V-Sync?

- Open the following file:

    ```cmd
    %APPDATA%\Minecraft Bedrock\Users\*\games\com.mojang\minecraftpe\options.txt
    ```

- Set `gfx_vsync` to `0`, save the file & launch the game.

## Build

1. Install [CMake](https://cmake.org) & [MinGW (x64)](https://www.mingw-w64.org).

2. Run [`BUILD.cmd`](BUILD.cmd) to build the project.