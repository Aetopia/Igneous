#pragma once

#define INITGUID
#define COBJMACROS
#define _MINAPPMODEL_H_
#define WIDL_C_INLINE_WRAPPERS

#include <windows.h>
#include <appmodel.h>
#include <shobjidl.h>

typedef struct Minecraft
{
    WCHAR ApplicationUserModelId[APPLICATION_USER_MODEL_ID_MAX_LENGTH];
    WCHAR PackageFamilyName[PACKAGE_FAMILY_NAME_MAX_LENGTH + 1];
    WCHAR Path[MAX_PATH];
} Minecraft;

extern IPackageDebugSettings *Settings;

extern IApplicationActivationManager *Manager;

__declspec(dllexport) BOOL WINAPI Minecraft_get_Installed(Minecraft *);

__declspec(dllexport) BOOL WINAPI Minecraft_get_Running(Minecraft *);

__declspec(dllexport) HRESULT WINAPI Minecraft_Launch(Minecraft *, PDWORD);

__declspec(dllexport) HRESULT WINAPI Minecraft_set_Debug(Minecraft *, BOOL);

__declspec(dllexport) HRESULT WINAPI Minecraft_Terminate(Minecraft *);

__declspec(dllexport) Minecraft *WINAPI Minecraft_get_Release(VOID);

__declspec(dllexport) Minecraft *WINAPI Minecraft_get_Preview(VOID);