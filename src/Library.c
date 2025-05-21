#include "Igneous.h"

Minecraft Release = {.ApplicationUserModelId = L"Microsoft.MinecraftUWP_8wekyb3d8bbwe!App",
                     .PackageFamilyName = L"Microsoft.MinecraftUWP_8wekyb3d8bbwe"};

Minecraft Preview = {.ApplicationUserModelId = L"Microsoft.MinecraftWindowsBeta_8wekyb3d8bbwe!App",
                     .PackageFamilyName = L"Microsoft.MinecraftWindowsBeta_8wekyb3d8bbwe"};

INIT_ONCE _ = INIT_ONCE_STATIC_INIT;

BOOL WINAPI Initialize(PINIT_ONCE InitOnce, PVOID Parameter, PVOID *Context)
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

Minecraft *WINAPI Minecraft_get_Release()
{
    InitOnceExecuteOnce(&_, Initialize, NULL, NULL);
    return &Release;
}

Minecraft *WINAPI Minecraft_get_Preview()
{
    InitOnceExecuteOnce(&_, Initialize, NULL, NULL);
    return &Preview;
}
