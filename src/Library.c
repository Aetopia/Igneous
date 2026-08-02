#include <minhook.h>
#include <dxgi1_4.h>

struct
{
    WNDPROC WindowProc;
    PEXCEPTION_ROUTINE CxxFrameHandler;

    BOOL (*ClipCursor)(PVOID);
    ATOM (*RegisterClassExW)(PVOID);
    HRESULT (*Present)(PVOID, UINT, UINT);
    HRESULT (*ResizeBuffers)(PVOID, UINT, UINT, UINT, DXGI_FORMAT, UINT);
    HRESULT (*CreateSwapChainForHwnd)(PVOID, PVOID, HWND, PVOID, PVOID, PVOID, PVOID);
    HRESULT (*ResizeBuffers1)(PVOID, UINT, UINT, UINT, DXGI_FORMAT, UINT, PVOID, PVOID);

    HWND hWnd;
    BOOL bClipped;
} _ = {};

PVOID __wrap_memcpy(PVOID Destination, PVOID Source, SIZE_T Count)
{
    __movsb(Destination, Source, Count);
    return Destination;
}

PVOID __wrap_memset(PVOID Destination, BYTE Data, SIZE_T Count)
{
    __stosb(Destination, Data, Count);
    return Destination;
}

__declspec(dllexport) EXCEPTION_DISPOSITION __CxxFrameHandler4(PVOID pExcept, PVOID pRN, PVOID pContext, PVOID pDC)
{
    return _.CxxFrameHandler(pExcept, pRN, pContext, pDC);
}

HRESULT $Present(PVOID This, UINT SyncInterval, UINT Flags)
{
    if (!SyncInterval)
        Flags |= DXGI_PRESENT_ALLOW_TEARING;
    return _.Present(This, SyncInterval, Flags);
}

HRESULT $ResizeBuffers(PVOID This, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat,
                       UINT SwapChainFlags)
{
    return _.ResizeBuffers(This, BufferCount, Width, Height, NewFormat,
                           SwapChainFlags |= DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING);
}

HRESULT $ResizeBuffers1(PVOID This, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT Format, UINT SwapChainFlags,
                        PVOID pCreationNodeMask, PVOID ppPresentQueue)
{
    return _.ResizeBuffers1(This, BufferCount, Width, Height, Format,
                            SwapChainFlags |= DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING, pCreationNodeMask, ppPresentQueue);
}

HRESULT $CreateSwapChainForHwnd(PVOID This, PVOID pDevice, HWND hWnd, DXGI_SWAP_CHAIN_DESC1 *pDesc,
                                PVOID pFullscreenDesc, PVOID pRestrictToOutput, IDXGISwapChain3 **ppSwapChain)
{
    pDesc->Flags |= DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;
    HRESULT hResult =
        _.CreateSwapChainForHwnd(This, pDevice, hWnd, pDesc, pFullscreenDesc, pRestrictToOutput, ppSwapChain);

    static BOOL bHooked = {};

    if (SUCCEEDED(hResult) && !bHooked)
    {
        bHooked = TRUE;
        _.hWnd = hWnd;

        MH_CreateHook((*ppSwapChain)->lpVtbl->Present, $Present, (PVOID)&_.Present);
        MH_CreateHook((*ppSwapChain)->lpVtbl->ResizeBuffers, $ResizeBuffers, (PVOID)&_.ResizeBuffers);
        MH_CreateHook((*ppSwapChain)->lpVtbl->ResizeBuffers1, $ResizeBuffers1, (PVOID)&_.ResizeBuffers1);

        MH_EnableHook(MH_ALL_HOOKS);
    }

    return hResult;
}

BOOL $SetCursorPos(INT X, INT Y)
{
    return FALSE;
}

HCURSOR $SetCursor(HCURSOR hCursor)
{
    return NULL;
}

BOOL $ClipCursor(PRECT pRect)
{
    _.bClipped = !!pRect;

    if (pRect)
    {
        GetClientRect(_.hWnd, pRect);
        pRect->top = (pRect->bottom - pRect->top) / 2;
        pRect->left = (pRect->right - pRect->left) / 2;

        ClientToScreen(_.hWnd, (PPOINT)pRect);
        pRect->right = pRect->left;
        pRect->bottom = pRect->top;
    }

    return _.ClipCursor(pRect);
}

LRESULT $WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_WINDOWPOSCHANGED:
        if (_.bClipped)
            ClipCursor(&(RECT){});
        break;

    case WM_SYSCOMMAND:
        switch (GET_SC_WPARAM(wParam))
        {
        case SC_KEYMENU:
        case SC_MOUSEMENU:
            return 0;
        }
        break;
    }

    return CallWindowProcW(_.WindowProc, hWnd, uMsg, wParam, lParam);
}

ATOM $RegisterClassExW(PWNDCLASSEXW pClass)
{
    static BOOL bHooked = {};

    if (CompareStringOrdinal(L"Bedrock", -1, pClass->lpszClassName, -1, FALSE) == CSTR_EQUAL && !bHooked)
    {
        bHooked = TRUE;

        _.WindowProc = pClass->lpfnWndProc;
        pClass->lpfnWndProc = $WindowProc;

        pClass->hCursor = LoadCursorW(NULL, IDC_ARROW);
        pClass->hbrBackground = GetStockObject(BLACK_BRUSH);

        MH_CreateHook(SetCursor, (PVOID)$SetCursor, NULL);
        MH_CreateHook(SetCursorPos, (PVOID)$SetCursorPos, NULL);
        MH_CreateHook(ClipCursor, $ClipCursor, (PVOID)&_.ClipCursor);

        IDXGIFactory2 *pFactory = {};
        CreateDXGIFactory(&IID_IDXGIFactory2, (PVOID)&pFactory);

        MH_CreateHook(pFactory->lpVtbl->CreateSwapChainForHwnd, $CreateSwapChainForHwnd,
                      (PVOID)&_.CreateSwapChainForHwnd);

        MH_EnableHook(MH_ALL_HOOKS);
        pFactory->lpVtbl->Release(pFactory);
    }

    return _.RegisterClassExW(pClass);
}

BOOL DllMain(HINSTANCE hInstance, DWORD dwReason, PVOID pReserved)
{
    if (dwReason == DLL_PROCESS_ATTACH)
    {
        DisableThreadLibraryCalls(hInstance);
        _.CxxFrameHandler = (PVOID)GetProcAddress(GetModuleHandleW(L"UCRTBASE"), "__CxxFrameHandler4");

        MH_Initialize();
        MH_CreateHook(RegisterClassExW, &$RegisterClassExW, (PVOID)&_.RegisterClassExW);
        MH_EnableHook(MH_ALL_HOOKS);
    }
    return TRUE;
}