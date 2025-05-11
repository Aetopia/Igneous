#pragma once

#include <minwindef.h>

#ifdef __cplusplus
extern "C"
{
#endif

    /*
        Check if Minecraft is installed.
        [In] Handle to Minecraft.
    */

    BOOL WINAPI Game_get_Installed(PVOID);

    /*
        Check if Minecraft is running.
        [In] Handle to Minecraft.
    */

    BOOL WINAPI Game_get_Running(PVOID);

    /*
        Launch Minecraft.
        [In] Handle to Minecraft.
        [Out] The process identifer.
    */

    HRESULT WINAPI Game_Launch(PVOID, PDWORD);

    /*
        Configure debug mode for Minecraft.
        [In] Handle to Minecraft.
        [In] TRUE to enable or FALSE to disable.
    */
    HRESULT WINAPI Game_set_Debug(PVOID, BOOL);

    /*
        Terminate Minecraft.
        [In] Handle to Minecraft.
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