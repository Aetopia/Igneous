#pragma once

#define INITGUID
#define COBJMACROS
#define _MINAPPMODEL_H_
#define WIDL_C_INLINE_WRAPPERS

#include <windows.h>
#include <appmodel.h>
#include <shobjidl.h>

typedef struct Game
{
    WCHAR ApplicationUserModelId[APPLICATION_USER_MODEL_ID_MAX_LENGTH];
    WCHAR PackageFamilyName[PACKAGE_FAMILY_NAME_MAX_LENGTH + 1];
    WCHAR Path[MAX_PATH];
} Game;

extern IPackageDebugSettings *Settings;

extern IApplicationActivationManager *Manager;