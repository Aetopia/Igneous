#pragma once

#define INITGUID
#define COBJMACROS
#define _MINAPPMODEL_H_
#define WIDL_C_INLINE_WRAPPERS

#include <windows.h>
#include <appmodel.h>
#include <shobjidl.h>

typedef struct
{
    WCHAR ApplicationUserModelId[APPLICATION_USER_MODEL_ID_MAX_LENGTH];
    WCHAR PackageFamilyName[PACKAGE_FAMILY_NAME_MAX_LENGTH + 1];
    WCHAR Path[MAX_PATH];
} Igneous_App;

extern IPackageDebugSettings *Igneous_App_Settings;

extern IApplicationActivationManager *Igneous_App_Manager;

__declspec(dllexport) BOOL Igneous_App_get_Installed(Igneous_App *);

__declspec(dllexport) BOOL Igneous_App_get_Running(Igneous_App *);

__declspec(dllexport) HRESULT Igneous_App_Launch(Igneous_App *, PDWORD);

__declspec(dllexport) HRESULT Igneous_App_Terminate(Igneous_App *);

__declspec(dllexport) HRESULT Igneous_Game_Launch(Igneous_App *, PDWORD value);

__declspec(dllexport) VOID Igneous_Library_Initialize();

__declspec(dllexport) VOID Igneous_Library_Uninitialize();

__declspec(dllexport) extern Igneous_App Igneous_Minecraft_Release;

__declspec(dllexport) extern Igneous_App Igneous_Minecraft_Preview;