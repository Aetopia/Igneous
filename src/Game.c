#include   "include/Igneous.h"

HRESULT Igneous_Game_Launch(Igneous_App *this, PDWORD value)
{
    HANDLE file = CreateFile2(this->Path, (DWORD){}, FILE_SHARE_DELETE, OPEN_EXISTING, NULL);

    if (!Igneous_App_get_Running(this) || file != INVALID_HANDLE_VALUE)
    {
        HRESULT result = Igneous_App_Launch(this, value);
        if (FAILED(result))
            goto _;

        HANDLE process = OpenProcess(SYNCHRONIZE, FALSE, *value);

        while (file == INVALID_HANDLE_VALUE)
        {
            if (WaitForSingleObject(process, TRUE) != WAIT_TIMEOUT)
            {
                result = S_FALSE;
                goto _;
            }
            file = CreateFile2(this->Path, (DWORD){}, FILE_SHARE_DELETE, OPEN_EXISTING, NULL);
        }

        FILE_STANDARD_INFO info = {};

        while (TRUE)
        {
            if (WaitForSingleObject(process, TRUE) != WAIT_TIMEOUT)
            {
                result = S_FALSE;
                goto _;
            }

            GetFileInformationByHandleEx(file, FileStandardInfo, &info, sizeof(info));
            if (info.DeletePending)
                break;
        }

    _:
        CloseHandle(file);
        CloseHandle(process);
        return result;
    }

    return Igneous_App_Launch(this, value);
}