#pragma once

#define _MINAPPMODEL_H_
#define COBJMACROS
#define WIDL_C_INLINE_WRAPPERS

#include <windows.h>
#include <appmodel.h>
#include <shobjidl.h>
#include <shlobj.h>

typedef struct App
{
    WCHAR Id[APPLICATION_USER_MODEL_ID_MAX_LENGTH];
    WCHAR Name[PACKAGE_FAMILY_NAME_MAX_LENGTH + 1];
    WCHAR File[MAX_PATH];
} App;

IApplicationActivationManager *Igneous_App_Manager = {};

IPackageDebugSettings *Igneous_App_Settings = {};

App Igneous_Minecraft_Release = {.Id = L"Microsoft.MinecraftUWP_8wekyb3d8bbwe!App",
                                 .Name = L"Microsoft.MinecraftUWP_8wekyb3d8bbwe"};

App Igneous_Minecraft_Preview = {.Id = L"Microsoft.MinecraftWindowsBeta_8wekyb3d8bbwe!App",
                                 .Name = L"Microsoft.MinecraftWindowsBeta_8wekyb3d8bbwe"};

BOOL Igneous_Load();

BOOL Igneous_Free();

HRESULT Igneous_App_Launch(App *, PDWORD);

HRESULT Igneous_App_Terminate(App *);