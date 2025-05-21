#include "Igneous.h"

IPackageDebugSettings *Settings = {};

IApplicationActivationManager *Manager = {};

BOOL WINAPI Minecraft_get_Installed(Minecraft *this)
{
    UINT32 value = {};
    GetPackagesByPackageFamily(this->PackageFamilyName, &value, NULL, &(UINT32){}, NULL);
    return value;
}

BOOL WINAPI Minecraft_get_Running(Minecraft *this)
{
    HWND hWnd = {};
    WCHAR szString[APPLICATION_USER_MODEL_ID_MAX_LENGTH] = {};

    while ((hWnd = FindWindowExW(NULL, hWnd, L"MSCTFIME UI", NULL)))
    {
        DWORD dwProcessId = {};
        GetWindowThreadProcessId(hWnd, &dwProcessId);

        HANDLE hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, dwProcessId);

        if (!GetApplicationUserModelId(hProcess, &(UINT32){ARRAYSIZE(szString)}, szString) &&
            CompareStringOrdinal(szString, -1, this->ApplicationUserModelId, -1, TRUE) == CSTR_EQUAL)
        {
            CloseHandle(hProcess);
            return TRUE;
        }

        CloseHandle(hProcess);
    }

    return FALSE;
}

HRESULT WINAPI Minecraft_Launch(Minecraft *this, PDWORD value)
{
    HANDLE hFile = CreateFile2(this->Path, (DWORD){}, FILE_SHARE_DELETE, OPEN_EXISTING, NULL);

    if (!Minecraft_get_Running(this) || hFile != INVALID_HANDLE_VALUE)
    {
        HRESULT hResult = IApplicationActivationManager_ActivateApplication(Manager, this->ApplicationUserModelId, NULL,
                                                                            AO_NOERRORUI, value);
        if (FAILED(hResult))
            goto _;

        HANDLE hProcess = OpenProcess(SYNCHRONIZE, FALSE, *value);

        while (hFile == INVALID_HANDLE_VALUE)
            if (WaitForSingleObject(hProcess, TRUE) != WAIT_TIMEOUT)
            {
                hResult = S_FALSE;
                goto _;
            }
            else
                hFile = CreateFile2(this->Path, (DWORD){}, FILE_SHARE_DELETE, OPEN_EXISTING, NULL);

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

    return IApplicationActivationManager_ActivateApplication(Manager, this->ApplicationUserModelId, NULL, AO_NOERRORUI,
                                                             value);
}

HRESULT WINAPI Minecraft_set_Debug(Minecraft *this, BOOL value)
{
    WCHAR packageFullName[PACKAGE_FULL_NAME_MAX_LENGTH + 1] = {};
    GetPackagesByPackageFamily(this->PackageFamilyName, &(UINT32){PACKAGE_GRAPH_MIN_SIZE}, &(PWSTR){},
                               &(UINT32){ARRAYSIZE(packageFullName)}, packageFullName);

    return value ? IPackageDebugSettings_EnableDebugging(Settings, packageFullName, NULL, NULL)
                 : IPackageDebugSettings_DisableDebugging(Settings, packageFullName);
}

HRESULT WINAPI Minecraft_Terminate(Minecraft *this)
{
    WCHAR packageFullName[PACKAGE_FULL_NAME_MAX_LENGTH + 1] = {};
    GetPackagesByPackageFamily(this->PackageFamilyName, &(UINT32){PACKAGE_GRAPH_MIN_SIZE}, &(PWSTR){},
                               &(UINT32){ARRAYSIZE(packageFullName)}, packageFullName);

    return IPackageDebugSettings_TerminateAllProcesses(Settings, packageFullName);
}