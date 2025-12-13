#include <minhook.h>
#include <dxgi1_4.h>

BOOL (*_ClipCursor)(PVOID) = {};
ATOM (*_RegisterClassExW)(PVOID) = {};
HRESULT (*_Present)(PVOID, UINT, UINT) = {};
HRESULT (*_ResizeBuffers)(PVOID, UINT, UINT, UINT, DXGI_FORMAT, UINT) = {};
HRESULT (*_CreateSwapChainForHwnd)(PVOID, PVOID, HWND, PVOID, PVOID, PVOID, PVOID) = {};
HRESULT (*_ResizeBuffers1)(PVOID, UINT, UINT, UINT, DXGI_FORMAT, UINT, PVOID, PVOID) = {};

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
    static PEXCEPTION_HANDLER __CxxFrameHandler4 = {};

    if (!__CxxFrameHandler4)
    {
        HMODULE hModule = GetModuleHandleW(L"ucrtbase");
        __CxxFrameHandler4 = (PVOID)GetProcAddress(hModule, "__CxxFrameHandler4");
    }

    return __CxxFrameHandler4(pExcept, pRN, pContext, pDC);
}

LRESULT $WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    static WNDPROC WindowProc = {};

    if (!WindowProc)
    {
        SetClassLongPtrW(hWnd, GCLP_HCURSOR, (LONG_PTR)LoadCursorW(NULL, IDC_ARROW));
        WindowProc = (PVOID)SetWindowLongPtrW(hWnd, GWLP_WNDPROC, (LONG_PTR)$WindowProc);
    }

    if (uMsg == WM_WINDOWPOSCHANGED)
    {
        GUITHREADINFO _ = {.cbSize = sizeof _};
        GetGUIThreadInfo(GetCurrentThreadId(), &_);

        if (_.hwndActive && _.hwndActive == _.hwndCapture && _.hwndActive != _.hwndMoveSize)
            ClipCursor(&(RECT){});
    }

    return CallWindowProcW(WindowProc, hWnd, uMsg, wParam, lParam);
}

HRESULT $Present(PVOID pSwapChain, UINT SyncInterval, UINT Flags)
{
    if (!SyncInterval)
        Flags |= DXGI_PRESENT_ALLOW_TEARING;
    return _Present(pSwapChain, SyncInterval, Flags);
}

HRESULT $ResizeBuffers(PVOID pSwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat,
                       UINT SwapChainFlags)
{
    SwapChainFlags |= DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;
    return _ResizeBuffers(pSwapChain, BufferCount, Width, Height, NewFormat, SwapChainFlags);
}

HRESULT $ResizeBuffers1(PVOID pSwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT Format,
                        UINT SwapChainFlags, PVOID pCreationNodeMask, PVOID ppPresentQueue)
{
    SwapChainFlags |= DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;
    return _ResizeBuffers1(pSwapChain, BufferCount, Width, Height, Format, SwapChainFlags, pCreationNodeMask,
                           ppPresentQueue);
}

HRESULT $CreateSwapChainForHwnd(PVOID pFactory, PVOID pDevice, HWND hWnd, DXGI_SWAP_CHAIN_DESC1 *pDesc,
                                PVOID pFullscreenDesc, PVOID pRestrictToOutput, IDXGISwapChain3 **ppSwapChain)
{
    static BOOL _ = {};
    pDesc->Flags |= DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;

    HRESULT hResult =
        _CreateSwapChainForHwnd(pFactory, pDevice, hWnd, pDesc, pFullscreenDesc, pRestrictToOutput, ppSwapChain);

    if (!_ && !hResult)
    {
        MH_CreateHook((*ppSwapChain)->lpVtbl->Present, $Present, (PVOID)&_Present);
        MH_CreateHook((*ppSwapChain)->lpVtbl->ResizeBuffers, $ResizeBuffers, (PVOID)&_ResizeBuffers);
        MH_CreateHook((*ppSwapChain)->lpVtbl->ResizeBuffers1, $ResizeBuffers1, (PVOID)&_ResizeBuffers1);

        _ = !MH_EnableHook(MH_ALL_HOOKS);
        $WindowProc(hWnd, WM_NULL, 0, 0);
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
    HWND hWnd = GetActiveWindow();

    if (hWnd && pRect)
    {
        GetClientRect(hWnd, pRect);
        pRect->top = (pRect->bottom - pRect->top) / 2;
        pRect->left = (pRect->right - pRect->left) / 2;

        ClientToScreen(hWnd, (PPOINT)pRect);
        pRect->right = pRect->left;
        pRect->bottom = pRect->top;
    }

    return _ClipCursor(pRect);
}

ATOM $RegisterClassExW(PWNDCLASSEXW pWndClass)
{
    static BOOL _ = {};

    if (!_)
    {
        MH_CreateHook(SetCursor, (PVOID)$SetCursor, NULL);
        MH_CreateHook(SetCursorPos, (PVOID)$SetCursorPos, NULL);
        MH_CreateHook(ClipCursor, $ClipCursor, (PVOID)&_ClipCursor);

        IDXGIFactory2 *pFactory = {};
        CreateDXGIFactory(&IID_IDXGIFactory2, (PVOID)&pFactory);

        PVOID CreateSwapChainForHwnd = pFactory->lpVtbl->CreateSwapChainForHwnd;
        MH_CreateHook(CreateSwapChainForHwnd, $CreateSwapChainForHwnd, (PVOID)&_CreateSwapChainForHwnd);

        _ = !MH_EnableHook(MH_ALL_HOOKS);
        pFactory->lpVtbl->Release(pFactory);
    }

    return _RegisterClassExW(pWndClass);
}

BOOL DllMain(HINSTANCE hInstance, DWORD dwReason, PVOID pReserved)
{
    if (dwReason == DLL_PROCESS_ATTACH)
    {
        DisableThreadLibraryCalls(hInstance);
        MH_Initialize();

        MH_CreateHook(RegisterClassExW, &$RegisterClassExW, (PVOID)&_RegisterClassExW);
        MH_EnableHook(MH_ALL_HOOKS);
    }
    return TRUE;
}