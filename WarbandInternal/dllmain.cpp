// dllmain.cpp : DLL uygulamasının giriş noktasını tanımlar.
#include "pch.h"
#include "includes.h"
#include "ImGui/imgui.h"
void* d3d9Device[119];
BYTE EndSceneBytes[7]{ 0 };
tEndScene oEndScene = nullptr;
extern LPDIRECT3DDEVICE9 pDevice = nullptr;
tReset oReset = nullptr;
extern boolean bReset = false;
typedef struct
{
    DWORD R;
    DWORD G;
    DWORD B;
    DWORD A;
}RGBA;
WNDPROC oWndProc;
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT __stdcall WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

    if (true && ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
        return true;

    return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
}
bool init = true;
void APIENTRY hkEndScene(LPDIRECT3DDEVICE9 o_pDevice)
{
    if (!pDevice)
        pDevice = o_pDevice;

    HRESULT cooperativeStatus = pDevice->TestCooperativeLevel();
    if (cooperativeStatus == D3DERR_DEVICELOST)
    {
        bReset = true;
        onLostDevice();
    }
    else if (cooperativeStatus == D3D_OK && bReset)
    {
        bReset = false;
        cout << "* D3D9Device reset!" << endl;
        onResetDevice();
    }
    if (bReset == true)
    {
        oEndScene(pDevice);
        return;
    }

    DWORD cur_mission = 0x8b829c;
    DWORD tactical_window = 0xdd9b18;
    Tactical_Window* tw = (Tactical_Window*)(*(uintptr_t*)tactical_window);

    if (!cur_mission || !tactical_window || !tw)
    {
        oEndScene(pDevice);
        return;
    }
    if (init) {
        guiInit(pDevice);
        init = false;
    }
    
    guiUpdate();
    oEndScene(pDevice);
}
DWORD WINAPI Menue(HINSTANCE hModule) {
    AllocConsole();
    FILE* fp;
    freopen_s(&fp, "CONOUT$", "w", stdout);
    cout << "* Trying to hook D3D9..." << endl;
    if (GetD3D9Device(d3d9Device, sizeof(d3d9Device))) {
        memcpy(EndSceneBytes, (char*)d3d9Device[42], 7);
        cout << "* Trying to hook D3D9..." << endl;
        oEndScene = (tEndScene)TrampHook((char*)d3d9Device[42], (char*)hkEndScene, 7);
        oWndProc = (WNDPROC)SetWindowLongPtr(FindWindow(NULL, "Mount&Blade Warband"), GWL_WNDPROC, (LONG_PTR)WndProc);

    }
    while (!GetAsyncKeyState(VK_END) & 1)
    {
        Sleep(500);
    }

    cout << "* Unhook..." << endl;
    Patch((BYTE*)d3d9Device[42], EndSceneBytes, 7);
    cout << "* Unhook done!" << endl;
    FreeConsole();
    FreeLibraryAndExitThread(hModule, NULL);
    return 0;
}
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Menue, NULL, 0, NULL);
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

