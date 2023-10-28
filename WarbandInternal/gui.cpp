#include "includes.h"

using namespace ImGui;
int tab = 0;
bool visualTeam = false;
bool visualEnemy = false;
bool visualHorse = false;

bool visualEnemyEsp = false;
bool visualEnemyHealthbar = false;
bool visualEnemyLines = false;

bool visualHorseEsp = false;
bool visualHorseHealthbar = false;
bool visualHorseLines = false;

bool visualTeamEsp = false;
bool visualTeamHealthbar = false;
bool visualTeamLines = false;
bool show = true;
void guiInit(LPDIRECT3DDEVICE9 pDevice) {
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;
    ImGui_ImplWin32_Init(FindWindow(NULL, "Mount&Blade Warband"));
    ImGui_ImplDX9_Init(pDevice);
    Init();
}
void guiUpdate() {
    ImGui_ImplDX9_NewFrame();
    ImGui_ImplWin32_NewFrame();

    NewFrame();
    if (GetAsyncKeyState(VK_INSERT) & 1) {
        show = !show;
    }

    if (visualTeam) {
        visualTeamRender(visualTeamEsp, visualTeamHealthbar, visualTeamLines);
    }
    if (visualEnemy) {
        visualEnemyRender(visualEnemyEsp, visualEnemyHealthbar, visualEnemyLines);
    }
    if (visualHorse) {
        visualHorseRender(visualHorseEsp, visualHorseHealthbar, visualHorseLines);
    }
    if (!show) {
        End();
        Render();
        ImGui_ImplDX9_RenderDrawData(GetDrawData());
        return;
    }
    SetNextWindowSize(ImVec2(600, 450));
    Begin("Warband Cheat - dc: waytner", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);
    SetCursorPos(ImVec2(177, 29));
    Checkbox("ALLY", &visualTeam);
    
    SetCursorPos(ImVec2(197, 61));
    Checkbox("esp", &visualTeamEsp);
    SetCursorPos(ImVec2(197, 82));
    Checkbox("health bar", &visualTeamHealthbar);
    SetCursorPos(ImVec2(197, 103));
    Checkbox("lines", &visualTeamLines);

    SetCursorPos(ImVec2(327, 29));
    Checkbox("ENEMY", &visualEnemy);
    SetCursorPos(ImVec2(347, 61));
    Checkbox("esp##f1", &visualEnemyEsp);
    SetCursorPos(ImVec2(347, 82));
    Checkbox("health bar##f1", &visualEnemyHealthbar);
    SetCursorPos(ImVec2(347, 103));
    Checkbox("lines##f1", &visualEnemyLines);

    SetCursorPos(ImVec2(477, 29));
    Checkbox("HORSE", &visualHorse);
    SetCursorPos(ImVec2(497, 61));
    Checkbox("esp##f2", &visualHorseEsp);
    SetCursorPos(ImVec2(497, 82));
    Checkbox("health bar##f2", &visualHorseHealthbar);
    SetCursorPos(ImVec2(497, 103));
    Checkbox("lines##f3", &visualHorseLines);
    End();
    Render();

    ImGui_ImplDX9_RenderDrawData(GetDrawData());

}
