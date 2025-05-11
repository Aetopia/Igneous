#pragma once

#include <minwindef.h>

#ifdef __cplusplus
extern "C"
{
#endif

    /*
        Check if a game is installed.
        [In] Handle to a game.
    */

    BOOL WINAPI Game_get_Installed(PVOID);

    /*
        Check if a game is running.
        [In] Handle to a game.
    */

    BOOL WINAPI Game_get_Running(PVOID);

    /*
        Launch a game.
        [In] Handle to a game.
        [Out] The process identifer.
    */

    HRESULT WINAPI Game_Launch(PVOID, PDWORD);

    /*
        Configure debug mode for a game.
        [In] Handle to a game.
        [In] TRUE to enable or FALSE to disable.
    */
    HRESULT WINAPI Game_set_Debug(PVOID, BOOL);

    /*
        Terminate a game.
        [In] Handle to a game.
    */

    HRESULT WINAPI Game_Terminate(PVOID);

    /*
        Returns a handle for Minecraft.
    */

    PVOID WINAPI Minecraft_get_Release();

    /*
        Returns a handle for Minecraft: Preview.
    */

    PVOID WINAPI Minecraft_get_Preview();

#ifdef __cplusplus
}
#endif