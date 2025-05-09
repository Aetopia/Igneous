/*
    MIT License

    Copyright (c) 2025 Aetopia

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
*/

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
        Launches a game.
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