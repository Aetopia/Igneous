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

#include "Igneous.h"

Game Release = {.ApplicationUserModelId = L"Microsoft.MinecraftUWP_8wekyb3d8bbwe!App",
                .PackageFamilyName = L"Microsoft.MinecraftUWP_8wekyb3d8bbwe"};

Game Preview = {.ApplicationUserModelId = L"Microsoft.MinecraftWindowsBeta_8wekyb3d8bbwe!App",
                .PackageFamilyName = L"Microsoft.MinecraftWindowsBeta_8wekyb3d8bbwe"};

INIT_ONCE InitOnce = INIT_ONCE_STATIC_INIT;

BOOL WINAPI InitOnceCallback(PINIT_ONCE InitOnce, PVOID Parameter, PVOID *Context)
{
    ExpandEnvironmentStringsW(L"%LOCALAPPDATA%\\Packages\\Microsoft.MinecraftUWP_8wekyb3d8bbwe"
                              L"\\LocalState\\games\\com.mojang\\minecraftpe\\resource_init_lock",
                              Release.Path, MAX_PATH);

    ExpandEnvironmentStringsW(L"%LOCALAPPDATA%\\Packages\\Microsoft.MinecraftWindowsBeta_8wekyb3d8bbwe"
                              L"\\LocalState\\games\\com.mojang\\minecraftpe\\resource_init_lock",
                              Preview.Path, MAX_PATH);

    CoInitializeEx(NULL, COINITBASE_MULTITHREADED | COINIT_DISABLE_OLE1DDE);

    CoCreateInstance(&CLSID_ApplicationActivationManager, NULL, CLSCTX_INPROC_SERVER,
                     &IID_IApplicationActivationManager, (PVOID)&Manager);

    CoCreateInstance(&CLSID_PackageDebugSettings, NULL, CLSCTX_INPROC_SERVER, &IID_IPackageDebugSettings,
                     (PVOID)&Settings);

    return TRUE;
}

__declspec(dllexport) Game *WINAPI Minecraft_get_Release()
{
    InitOnceExecuteOnce(&InitOnce, InitOnceCallback, NULL, NULL);
    return &Release;
}

__declspec(dllexport) Game *WINAPI Minecraft_get_Preview()
{
    InitOnceExecuteOnce(&InitOnce, InitOnceCallback, NULL, NULL);
    return &Preview;
}
