#pragma once
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_win32.h"
#include "ImGui/imgui_impl_dx9.h"
void guiInit(LPDIRECT3DDEVICE9 pDevice);
void guiUpdate();
