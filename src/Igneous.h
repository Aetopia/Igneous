#pragma once

#define _MINAPPMODEL_H_
#define COBJMACROS
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

IApplicationActivationManager *Igneous_App_Manager;

IPackageDebugSettings *Igneous_App_Settings;

Igneous_App Igneous_Minecraft_Release;

Igneous_App Igneous_Minecraft_Preview;

BOOL Igneous_App_Running(Igneous_App *);

HRESULT Igneous_App_Launch(Igneous_App *, PDWORD);

HRESULT Igneous_App_Terminate(Igneous_App *);