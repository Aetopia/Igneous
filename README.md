# Igneous

> [!CAUTION]
> **Only supported on Windows x64!**

Fixes various bugs related to Minecraft: Bedrock Edition (GDK).

## Fixes

|Issue|Summary|
|-|-|
|[MCPE-15796](https://bugs.mojang.com/browse/MCPE-15796)|Cursor is not recentered upon the opening of a new gui|
|[MCPE-98861](https://bugs.mojang.com/browse/MCPE-98861)|Significant input delay on devices with Render Dragon|
|[MCPE-110006](https://bugs.mojang.com/browse/MCPE-110006)|Vsync not being able to be turned off|
|[MCPE-166745](https://bugs.mojang.com/browse/MCPE-166745)|FPS is capped at double the screen's refresh rate when v-sync is disabled|
|[MCPE-230832](https://bugs.mojang.com/browse/MCPE-230832)|Mouse cursor moves in the background during gameplay, disrupting gameplay|

### Tweaks

- Cursor changes are disallowed to prevent a change from lingering. 

- The game's window background is now black to prevent flashbangs.

- The game's window menu is now disabled preventing unwanted freezes.

- If locked, the cursor's position is updated when the game's window is moved.

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
1. Install & update [MSYS2](https://www.msys2.org):

    ```bash
    pacman -Syu --noconfirm
    ```

3. Install [GCC](https://gcc.gnu.org) & [MinHook](https://github.com/TsudaKageyu/minhook):

    ```bash
    pacman -Syu mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-MinHook --noconfirm
    ```


3. Start MSYS2's `UCRT64` environment & run `Build.cmd`.
