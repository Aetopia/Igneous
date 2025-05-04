#include "Igneous.h"

BOOL Initialized = {};

BOOL Igneous_Load()
{
    if (!Initialized)
    {
        Initialized = TRUE;

        CoInitializeEx(NULL, COINIT_MULTITHREADED | COINIT_DISABLE_OLE1DDE);

        CoCreateInstance(&CLSID_ApplicationActivationManager, NULL, CLSCTX_INPROC_SERVER,
                         &IID_IApplicationActivationManager, (PVOID)&Igneous_App_Manager);

        CoCreateInstance(&CLSID_PackageDebugSettings, NULL, CLSCTX_INPROC_SERVER, &IID_IPackageDebugSettings,
                         (PVOID)&Igneous_App_Settings);

        ExpandEnvironmentStringsW(L"%LOCALAPPDATA%\\Packages\\Microsoft.MinecraftUWP_"
                                  L"8wekyb3d8bbwe\\LocalState\\games\\com.mojang\\minecraftpe\\resource_init_lock",
                                  Igneous_Minecraft_Release.File, MAX_PATH);

        ExpandEnvironmentStringsW(L"%LOCALAPPDATA%\\Packages\\Microsoft.MinecraftWindowsBeta_"
                                  L"8wekyb3d8bbwe\\LocalState\\games\\com.mojang\\minecraftpe\\resource_init_lock",
                                  Igneous_Minecraft_Preview.File, MAX_PATH);
    }
    return Initialized;
}

BOOL Igneous_Free()
{
    if (Initialized)
    {
        Initialized = FALSE;

        IApplicationActivationManager_Release(Igneous_App_Manager);

        IPackageDebugSettings_Release(Igneous_App_Settings);

        CoUninitialize();
    }
    return Initialized;
}