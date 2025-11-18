#define INITGUID
#include <MinHook.h>
#include <shlwapi.h>
#include <dxgi1_6.h>

__declspec(dllexport) EXCEPTION_DISPOSITION __CxxFrameHandler4(PVOID pExcept, PVOID pRN, PVOID pContext, PVOID pDC)
{
    static EXCEPTION_DISPOSITION (*_)(PVOID, PVOID, PVOID, PVOID) = {};

    if (!_)
    {
        WCHAR szName[MAX_PATH] = {};

        GetSystemDirectoryW(szName, MAX_PATH);
        PathCombineW(szName, szName, L"vcruntime140_1.dll");

        _ = (PVOID)GetProcAddress(LoadLibraryW(szName), "__CxxFrameHandler4");
    }

    return _(pExcept, pRN, pContext, pDC);
}

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

BOOL (*_ClipCursor)(PVOID) = {};
ATOM (*_RegisterClassExW)(PVOID) = {};
HRESULT (*_Present)(PVOID, UINT, UINT) = {};
LRESULT (*_WndProc)(HWND, UINT, WPARAM, LPARAM) = {};
HRESULT (*_ResizeBuffers)(PVOID, UINT, UINT, UINT, DXGI_FORMAT, UINT) = {};
HRESULT (*_CreateSwapChainForHwnd)(PVOID, PVOID, HWND, PVOID, PVOID, PVOID, PVOID) = {};
HRESULT (*_ResizeBuffers1)(PVOID, UINT, UINT, UINT, DXGI_FORMAT, UINT, PVOID, PVOID) = {};

HRESULT $Present(PVOID pSwapChain, UINT SyncInterval, UINT Flags)
{
    if (!SyncInterval && !(Flags & DXGI_PRESENT_ALLOW_TEARING))
        Flags |= DXGI_PRESENT_ALLOW_TEARING;
    return _Present(pSwapChain, SyncInterval, Flags);
}

HRESULT $ResizeBuffers(PVOID pSwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat,
                       UINT SwapChainFlags)
{
    if (!(SwapChainFlags & DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING))
        SwapChainFlags |= DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;
    return _ResizeBuffers(pSwapChain, BufferCount, Width, Height, NewFormat, SwapChainFlags);
}

HRESULT $ResizeBuffers1(PVOID pSwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT Format,
                        UINT SwapChainFlags, PVOID pCreationNodeMask, PVOID ppPresentQueue)
{
    if (!(SwapChainFlags & DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING))
        SwapChainFlags |= DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;
    return _ResizeBuffers1(pSwapChain, BufferCount, Width, Height, Format, SwapChainFlags, pCreationNodeMask,
                           ppPresentQueue);
}

HRESULT $CreateSwapChainForHwnd(PVOID pFactory, PVOID pDevice, HWND hWnd, DXGI_SWAP_CHAIN_DESC1 *pDesc,
                                PVOID pFullscreenDesc, PVOID pRestrictToOutput, IDXGISwapChain4 **ppSwapChain)
{
    static BOOL _ = {};

    if (!(pDesc->Flags & DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING))
        pDesc->Flags |= DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;

    HRESULT hResult =
        _CreateSwapChainForHwnd(pFactory, pDevice, hWnd, pDesc, pFullscreenDesc, pRestrictToOutput, ppSwapChain);

    if (!_ && SUCCEEDED(hResult))
    {
        MH_CreateHook((*ppSwapChain)->lpVtbl->Present, &$Present, (PVOID)&_Present);
        MH_QueueEnableHook((*ppSwapChain)->lpVtbl->Present);

        MH_CreateHook((*ppSwapChain)->lpVtbl->ResizeBuffers, &$ResizeBuffers, (PVOID)&_ResizeBuffers);
        MH_QueueEnableHook((*ppSwapChain)->lpVtbl->ResizeBuffers);

        MH_CreateHook((*ppSwapChain)->lpVtbl->ResizeBuffers1, &$ResizeBuffers1, (PVOID)&_ResizeBuffers1);
        MH_QueueEnableHook((*ppSwapChain)->lpVtbl->ResizeBuffers1);

        MH_ApplyQueued();
        _ = TRUE;
    }

    return hResult;
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

LRESULT $WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (uMsg == WM_WINDOWPOSCHANGED)
    {
        HWND hWndActive = GetActiveWindow(), hWndCapture = GetCapture();
        if (hWndActive && hWndCapture && hWndActive == hWndCapture)
            ClipCursor(&(RECT){});
    }
    return _WndProc(hWnd, uMsg, wParam, lParam);
}

ATOM $RegisterClassExW(PWNDCLASSEXW pWndClass)
{
    static BOOL _ = {};

    if (!_ && CompareStringOrdinal(L"Bedrock", -1, pWndClass->lpszClassName, -1, FALSE) == CSTR_EQUAL)
    {
        _WndProc = pWndClass->lpfnWndProc;
        pWndClass->lpfnWndProc = $WndProc;
        pWndClass->hCursor = LoadCursorW(NULL, IDC_ARROW);

        IDXGIFactory7 *pFactory = {};
        CreateDXGIFactory2(0, &IID_IDXGIFactory7, (PVOID)&pFactory);

        MH_CreateHook(pFactory->lpVtbl->CreateSwapChainForHwnd, &$CreateSwapChainForHwnd,
                      (PVOID)&_CreateSwapChainForHwnd);
        MH_EnableHook(pFactory->lpVtbl->CreateSwapChainForHwnd);

        pFactory->lpVtbl->Release(pFactory);
        _ = TRUE;
    }

    return _RegisterClassExW(pWndClass);
}

BOOL DllMain(HINSTANCE hInstance, DWORD dwReason, PVOID pReserved)
{
    if (dwReason == DLL_PROCESS_ATTACH)
    {
        DisableThreadLibraryCalls(hInstance);
        MH_Initialize();

        MH_CreateHook(ClipCursor, &$ClipCursor, (PVOID)&_ClipCursor);
        MH_QueueEnableHook(ClipCursor);

        MH_CreateHook(RegisterClassExW, &$RegisterClassExW, (PVOID)&_RegisterClassExW);
        MH_QueueEnableHook(RegisterClassExW);

        MH_ApplyQueued();
    }
    return TRUE;
}