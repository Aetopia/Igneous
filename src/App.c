#include "Igneous.h"

BOOL Igneous_App_Running(Igneous_App *this)
{
    HWND window = {};
    WCHAR applicationUserModelId[APPLICATION_USER_MODEL_ID_MAX_LENGTH] = {};

    while ((window = FindWindowExW(NULL, window, L"MSCTFIME UI", NULL)))
    {
        DWORD processId = {};
        GetWindowThreadProcessId(window, &processId);
      
        HANDLE process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);

        if (!GetApplicationUserModelId(process, &(UINT32){ARRAYSIZE(applicationUserModelId)}, applicationUserModelId) &&
            CompareStringOrdinal(this->ApplicationUserModelId, -1, applicationUserModelId, -1, TRUE) == CSTR_EQUAL)
        {
            CloseHandle(process);
            return TRUE;
        }

        CloseHandle(process);
    }

    return FALSE;
}

HRESULT Igneous_App_Launch(Igneous_App *this, PDWORD value)
{
    return IApplicationActivationManager_ActivateApplication(Igneous_App_Manager, this->ApplicationUserModelId, NULL,
                                                             AO_NOERRORUI, value);
}

HRESULT Igneous_App_Terminate(Igneous_App *this)
{
    WCHAR packageFullName[PACKAGE_FAMILY_NAME_MAX_LENGTH + 1] = {};

    GetPackagesByPackageFamily(this->PackageFamilyName, &(UINT32){PACKAGE_GRAPH_MIN_SIZE}, &(PWSTR){},
                               &(UINT32){ARRAYSIZE(packageFullName)}, packageFullName);

    return IPackageDebugSettings_TerminateAllProcesses(Igneous_App_Settings, packageFullName);
}