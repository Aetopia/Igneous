#include <dwmapi.h>
#include <MinHook.h>
#include <dxgi1_4.h>
#include <stdbool.h>
#include <stdatomic.h>

struct
{
    WNDPROC WindowProc;
    PEXCEPTION_HANDLER CxxFrameHandler;

    BOOL (*ClipCursor)(PVOID);
    ATOM (*RegisterClassExW)(PVOID);

    HRESULT (*Present)(PVOID, UINT, UINT);
    HRESULT (*ResizeBuffers)(PVOID, UINT, UINT, UINT, DXGI_FORMAT, UINT);
    HRESULT (*CreateSwapChainForHwnd)(PVOID, PVOID, HWND, PVOID, PVOID, PVOID, PVOID);
    HRESULT (*ResizeBuffers1)(PVOID, UINT, UINT, UINT, DXGI_FORMAT, UINT, PVOID, PVOID);

    HWND hWnd;
    atomic_bool bClipped;
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
    SwapChainFlags |= DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;
    return _.ResizeBuffers(This, BufferCount, Width, Height, NewFormat, SwapChainFlags);
}

HRESULT $ResizeBuffers1(PVOID This, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT Format, UINT SwapChainFlags,
                        PVOID pCreationNodeMask, PVOID ppPresentQueue)
{
    SwapChainFlags |= DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;
    return _.ResizeBuffers1(This, BufferCount, Width, Height, Format, SwapChainFlags, pCreationNodeMask,
                            ppPresentQueue);
}

HRESULT $CreateSwapChainForHwnd(PVOID This, PVOID pDevice, HWND hWnd, DXGI_SWAP_CHAIN_DESC1 *pDesc,
                                PVOID pFullscreenDesc, PVOID pRestrictToOutput, IDXGISwapChain3 **ppSwapChain)
{
    pDesc->Flags |= DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;
    HRESULT hResult =
        _.CreateSwapChainForHwnd(This, pDevice, hWnd, pDesc, pFullscreenDesc, pRestrictToOutput, ppSwapChain);

    static atomic_flag bHooked = {};

    if (SUCCEEDED(hResult) && !atomic_flag_test_and_set(&bHooked))
    {
        _.hWnd = hWnd;

        MH_CreateHook((*ppSwapChain)->lpVtbl->Present, $Present, (PVOID)&_.Present);
        MH_CreateHook((*ppSwapChain)->lpVtbl->ResizeBuffers, $ResizeBuffers, (PVOID)&_.ResizeBuffers);
        MH_CreateHook((*ppSwapChain)->lpVtbl->ResizeBuffers1, $ResizeBuffers1, (PVOID)&_.ResizeBuffers1);

        MH_QueueEnableHook((*ppSwapChain)->lpVtbl->Present);
        MH_QueueEnableHook((*ppSwapChain)->lpVtbl->ResizeBuffers);
        MH_QueueEnableHook((*ppSwapChain)->lpVtbl->ResizeBuffers1);

        MH_ApplyQueued();
    }

    return hResult;
}

BOOL $ClipCursor(PRECT pRect)
{
    atomic_store(&_.bClipped, pRect);

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
        if (atomic_load(&_.bClipped))
            ClipCursor(&(RECT){});
        break;
    }
    return _.WindowProc(hWnd, uMsg, wParam, lParam);
}

ATOM $RegisterClassExW(PWNDCLASSEXW pClass)
{
    static atomic_flag bHooked = {};

    if (CompareStringOrdinal(L"Bedrock", -1, pClass->lpszClassName, -1, FALSE) == CSTR_EQUAL &&
        !atomic_flag_test_and_set(&bHooked))
    {
        _.WindowProc = pClass->lpfnWndProc;
        pClass->lpfnWndProc = $WindowProc;

        pClass->hCursor = LoadCursorW(NULL, IDC_ARROW);
        pClass->hbrBackground = GetStockObject(BLACK_BRUSH);

        IDXGIFactory2 *pFactory = {};
        CreateDXGIFactory(&IID_IDXGIFactory2, (PVOID)&pFactory);

        MH_CreateHook(pFactory->lpVtbl->CreateSwapChainForHwnd, $CreateSwapChainForHwnd,
                      (PVOID)&_.CreateSwapChainForHwnd);

        MH_EnableHook(pFactory->lpVtbl->CreateSwapChainForHwnd);
        pFactory->lpVtbl->Release(pFactory);
    }

    return _.RegisterClassExW(pClass);
}

BOOL DllMain(HINSTANCE hInstance, DWORD dwReason, PVOID pReserved)
{
    if (dwReason == DLL_PROCESS_ATTACH)
    {
        DisableThreadLibraryCalls(hInstance);

        HMODULE hModule = GetModuleHandleW(L"UCRTBASE");
        _.CxxFrameHandler = (PVOID)GetProcAddress(hModule, "__CxxFrameHandler4");

        MH_Initialize();
        MH_CreateHook(ClipCursor, $ClipCursor, (PVOID)&_.ClipCursor);
        MH_CreateHook(RegisterClassExW, $RegisterClassExW, (PVOID)&_.RegisterClassExW);

        MH_QueueEnableHook(ClipCursor);
        MH_QueueEnableHook(RegisterClassExW);
        MH_ApplyQueued();

        DwmEnableMMCSS(TRUE);
    }
    return TRUE;
}