#include "Igneous.h"

IPackageDebugSettings *Settings = {};

IApplicationActivationManager *Manager = {};

__declspec(dllexport) BOOL Game_get_Installed(Game *This)
{
    UINT32 Value = {};
    GetPackagesByPackageFamily(This->PackageFamilyName, &Value, NULL, &(UINT32){}, NULL);
    return Value;
}

__declspec(dllexport) BOOL Game_get_Running(Game *This)
{
    HWND hWnd = {};
    WCHAR ApplicationUserModelId[APPLICATION_USER_MODEL_ID_MAX_LENGTH] = {};

    while ((hWnd = FindWindowExW(NULL, hWnd, L"MSCTFIME UI", NULL)))
    {
        DWORD dwProcessId = {};
        GetWindowThreadProcessId(hWnd, &dwProcessId);

        HANDLE hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, dwProcessId);

        if (!GetApplicationUserModelId(hProcess, &(UINT32){ARRAYSIZE(ApplicationUserModelId)},
                                       ApplicationUserModelId) &&
            CompareStringOrdinal(ApplicationUserModelId, -1, This->ApplicationUserModelId, -1, TRUE) == CSTR_EQUAL)
        {
            CloseHandle(hProcess);
            return TRUE;
        }

        CloseHandle(hProcess);
    }

    return FALSE;
}

__declspec(dllexport) HRESULT Game_Launch(Game *This, PDWORD Value)
{
    HANDLE hFile = CreateFile2(This->Path, (DWORD){}, FILE_SHARE_DELETE, OPEN_EXISTING, NULL);

    if (!Game_get_Running(This) || hFile != INVALID_HANDLE_VALUE)
    {
        HRESULT hResult = IApplicationActivationManager_ActivateApplication(Manager, This->ApplicationUserModelId, NULL,
                                                                            AO_NOERRORUI, Value);
        if (FAILED(hResult))
            goto _;

        HANDLE hProcess = OpenProcess(SYNCHRONIZE, FALSE, *Value);

        while (hFile == INVALID_HANDLE_VALUE)
            if (WaitForSingleObject(hProcess, TRUE) != WAIT_TIMEOUT)
            {
                hResult = S_FALSE;
                goto _;
            }
            else
                hFile = CreateFile2(This->Path, (DWORD){}, FILE_SHARE_DELETE, OPEN_EXISTING, NULL);

        FILE_STANDARD_INFO FileInformation = {};

        while (TRUE)
            if (WaitForSingleObject(hProcess, TRUE) != WAIT_TIMEOUT)
            {
                hResult = S_FALSE;
                goto _;
            }
            else if (GetFileInformationByHandleEx(hFile, FileStandardInfo, &FileInformation,
                                                  sizeof(FILE_STANDARD_INFO)) &&
                     FileInformation.DeletePending)
                break;
    _:
        CloseHandle(hFile);
        CloseHandle(hProcess);
        return hResult;
    }

    return IApplicationActivationManager_ActivateApplication(Manager, This->ApplicationUserModelId, NULL, AO_NOERRORUI,
                                                             Value);
}

__declspec(dllexport) HRESULT Game_set_Debug(Game *This, BOOL Value)
{
    WCHAR PackageFullName[PACKAGE_FULL_NAME_MAX_LENGTH + 1] = {};
    GetPackagesByPackageFamily(This->PackageFamilyName, &(UINT32){PACKAGE_GRAPH_MIN_SIZE}, &(PWSTR){},
                               &(UINT32){ARRAYSIZE(PackageFullName)}, PackageFullName);

    return Value ? IPackageDebugSettings_EnableDebugging(Settings, PackageFullName, NULL, NULL)
                 : IPackageDebugSettings_DisableDebugging(Settings, PackageFullName);
}

__declspec(dllexport) HRESULT Game_Terminate(Game *This)
{
    WCHAR PackageFullName[PACKAGE_FULL_NAME_MAX_LENGTH + 1] = {};
    GetPackagesByPackageFamily(This->PackageFamilyName, &(UINT32){PACKAGE_GRAPH_MIN_SIZE}, &(PWSTR){},
                               &(UINT32){ARRAYSIZE(PackageFullName)}, PackageFullName);

    return IPackageDebugSettings_TerminateAllProcesses(Settings, PackageFullName);
}