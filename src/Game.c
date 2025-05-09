/*
    MIT License

    Copyright (c) 2025 Aetopia

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
*/

#include "Igneous.h"

IPackageDebugSettings *Settings = {};

IApplicationActivationManager *Manager = {};

__declspec(dllexport) BOOL WINAPI Game_get_Installed(Game *This)
{
    UINT32 Value = {};
    GetPackagesByPackageFamily(This->PackageFamilyName, &Value, NULL, &(UINT32){}, NULL);
    return Value;
}

__declspec(dllexport) BOOL WINAPI Game_get_Running(Game *This)
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

__declspec(dllexport) HRESULT WINAPI Game_Launch(Game *This, PDWORD Value)
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

__declspec(dllexport) HRESULT WINAPI Game_set_Debug(Game *This, BOOL Value)
{
    WCHAR PackageFullName[PACKAGE_FULL_NAME_MAX_LENGTH + 1] = {};
    GetPackagesByPackageFamily(This->PackageFamilyName, &(UINT32){PACKAGE_GRAPH_MIN_SIZE}, &(PWSTR){},
                               &(UINT32){ARRAYSIZE(PackageFullName)}, PackageFullName);

    return Value ? IPackageDebugSettings_EnableDebugging(Settings, PackageFullName, NULL, NULL)
                 : IPackageDebugSettings_DisableDebugging(Settings, PackageFullName);
}

__declspec(dllexport) HRESULT WINAPI Game_Terminate(Game *This)
{
    WCHAR PackageFullName[PACKAGE_FULL_NAME_MAX_LENGTH + 1] = {};
    GetPackagesByPackageFamily(This->PackageFamilyName, &(UINT32){PACKAGE_GRAPH_MIN_SIZE}, &(PWSTR){},
                               &(UINT32){ARRAYSIZE(PackageFullName)}, PackageFullName);

    return IPackageDebugSettings_TerminateAllProcesses(Settings, PackageFullName);
}