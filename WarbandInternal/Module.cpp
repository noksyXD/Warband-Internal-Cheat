#include "includes.h"
#include "Module.h"
typedef float* (__thiscall* get_screen_pos_from_world_pos_t)(void* t, float* vec3, float* vec32);
get_screen_pos_from_world_pos_t get_screen_pos_from_world_pos;

typedef bool(__thiscall* is_enemy_t)(Agent* t, Agent* tr);
is_enemy_t is_enemy;
void Init() {
    get_screen_pos_from_world_pos = (get_screen_pos_from_world_pos_t)(0x53eef0);
    is_enemy = (is_enemy_t)(0x4adcc0);
}
Agent* get_agent(int id) {
    DWORD cur_mission = 0x8b829c;
    size_t* agentlist_array = *(size_t**)(*(uintptr_t*)cur_mission + 0x20);

    size_t agentlist_size = *(size_t*)(*(uintptr_t*)cur_mission + 0x8);
    if (id > agentlist_size) {
        return nullptr;
    }

    if (!agentlist_array) {
        return nullptr;
    }
    size_t agentlist = agentlist_array[id >> 4];
    Agent* agent = (Agent*)((size_t)(id & 0xf) * 0x6200 + agentlist); 
    if (agent && agent->is_valid != 0) { 
        return agent;
    }
    else
        return nullptr;
}
Agent* getLocalAgent() {
    DWORD cur_mission = 0x8b829c;
    size_t agentlist_size = *(size_t*)(*(uintptr_t*)cur_mission + 0x8);
    if (agentlist_size == 0)
        return nullptr;
    for (int i = 0; i < agentlist_size; i++) {
        Agent* agent = get_agent(i);
        if (!agent)
            continue;
        if (agent->isLocalAgent == 1) {
            return agent;
        }
    }
    return nullptr;
}
void worldToScreen(D3DXVECTOR3 world_pos, float* output)
{
    DWORD tactical_window = 0xdd9b18;
    Tactical_Window* tw = (Tactical_Window*)(*(uintptr_t*)tactical_window);
    if (tw->pScene_widget == 0) {
        return;
    }
    float* screen_pos = get_screen_pos_from_world_pos((void*)(tw->pScene_widget), world_pos, world_pos);

    if (screen_pos[0] < 0 || screen_pos[0]>1 || screen_pos[1] < 0 || screen_pos[1]>1) {
        return;
    }

    output[0] = screen_pos[0] * windowWidth;
    output[1] = windowHeight - windowHeight * (screen_pos[1] * 1.33);
}
void visualTeamRender(bool esp, bool healthbar, bool lines) {
    DWORD cur_mission = 0x8b829c;
    Agent* localAgent = getLocalAgent();
	size_t agentlist_size = *(size_t*)(*(uintptr_t*)cur_mission + 0x8);
	for (int i = 0; i < agentlist_size; i++) {
		Agent* agent = get_agent(i);
        if (!agent)
            continue;
        if (!localAgent)
            continue;
        if (localAgent && localAgent == agent)
            continue;
        D3DXVECTOR3 pos = agent->position;
        float agent_screen_pos[2] = { -1 };
        worldToScreen(pos, agent_screen_pos);
        if (agent_screen_pos[0] == -1) {
            continue;
        }
        if (agent->isHorse == 1) {
            continue;
        }
        rglStrategic_Entity* ent = agent->prgl_strat_ent;
        if (ent) {
            if (ent->ent_min && ent->ent_max && ent->ent_origin) {
                D3DXVECTOR3 min = ent->ent_min;
                D3DXVECTOR3 max = ent->ent_max;
                float mins[2] = { -1 };
                worldToScreen(min, mins);
                float maxs[2] = { -1 };
                worldToScreen(max, maxs);
                float width = fabs(mins[1] - maxs[1]) / 4;
                if (mins[0] != -1 && maxs[0] != -1) {
                    if (localAgent && is_enemy(localAgent, agent))
                        continue;
                    if (esp) {
                        ImVec4 boxColor(255, 255, 0, 255);
                        auto drawlist = ImGui::GetOverlayDrawList();
                        drawlist->AddRect(ImVec2(agent_screen_pos[0] - width, mins[1]), ImVec2(agent_screen_pos[0] + width, maxs[1]), ImColor(boxColor), 0.0f, 0, 1);
                        drawlist->AddRect(ImVec2(agent_screen_pos[0] - width, maxs[1]), ImVec2(agent_screen_pos[0] + width, mins[1]), ImColor(boxColor), 0.0f, 0, 1);
                        float thickness = 1;
                        drawlist->AddLine(ImVec2(agent_screen_pos[0] - width, mins[1]), ImVec2(agent_screen_pos[0] + width, mins[1]), ImColor(0, 0, 0, 255), 1);
                        drawlist->AddLine(ImVec2(agent_screen_pos[0] - width, maxs[1]), ImVec2(agent_screen_pos[0] + width, maxs[1]), ImColor(0, 0, 0, 255), 1);
                        drawlist->AddLine(ImVec2(agent_screen_pos[0] - width, mins[1]), ImVec2(agent_screen_pos[0] - width, maxs[1]), ImColor(0, 0, 0, 255), 1);
                        drawlist->AddLine(ImVec2(agent_screen_pos[0] + width, mins[1]), ImVec2(agent_screen_pos[0] + width, maxs[1]), ImColor(0, 0, 0, 255), 1);
                    }
                    if (healthbar) {
                        const ImVec2 barMin = ImVec2(agent_screen_pos[0] - width - 5, mins[1]);
                        const ImVec2 barMax = ImVec2{ agent_screen_pos[0] - width - 2, maxs[1] };

                        const float green = (agent->health / agent->max_health);
                        const float greenClamped = (std::min)(255.f, green);
                        const float red = (std::min)(255.f, 1 - green);

                        float val = (agent->health / agent->max_health);
                        const float height = val * (barMin.y - barMax.y);
                        float h = val * (width * 4);
                        float barHeight = val * height;
                        auto drawlist = ImGui::GetOverlayDrawList();
                        drawlist->AddRectFilled(ImVec2(barMin.x, barMin.y - 1),
                            ImVec2(barMax.x, barMax.y),
                            IM_COL32(0, 0, 0, 255));
                        float lineWidth = max(width / 4, 1.f);
                        drawlist->AddRectFilled(
                            ImVec2{ barMin.x + 1, mins[1] - h },
                            ImVec2{ (barMax.x - 1), barMin.y - 1 },
                            IM_COL32((int)((1 - val) * 255), (int)(val * 255), 0, 255));
                    }
                    if (lines) {
                        D3DCOLOR color = D3DCOLOR_ARGB(255, 0, 255, 0);
                        DrawLine(windowWidth / 2, windowHeight, agent_screen_pos[0], mins[1], 1, color);
                    }
                }
            }
        }
    }
}
void visualEnemyRender(bool esp, bool healthbar, bool lines) {
    DWORD cur_mission = 0x8b829c;
    size_t agentlist_size = *(size_t*)(*(uintptr_t*)cur_mission + 0x8);
    for (int i = 0; i < agentlist_size; i++) {
        Agent* agent = get_agent(i);
        if (!agent)
            continue;
        Agent* localAgent = getLocalAgent();
        if (localAgent && localAgent == agent)
            continue;
        D3DXVECTOR3 pos = agent->position;
        float agent_screen_pos[2] = { -1 };
        worldToScreen(pos, agent_screen_pos);

        if (agent_screen_pos[0] == -1) {
            continue;
        }
        if (agent->isHorse == 1) {
            continue;
        }
        rglStrategic_Entity* ent = agent->prgl_strat_ent;
        if (ent) {
            if (ent->ent_min && ent->ent_max && ent->ent_origin) {
                D3DXVECTOR3 min = ent->ent_min;
                D3DXVECTOR3 max = ent->ent_max;
                float mins[2] = { -1 };
                worldToScreen(min, mins);
                float maxs[2] = { -1 };
                worldToScreen(max, maxs);
                float width = fabs(mins[1] - maxs[1]) / 4;
                if (mins[0] != -1 && maxs[0] != -1) {
                    if (localAgent && !is_enemy(localAgent, agent))
                        continue;
                    if (esp) {
                        ImVec4 boxColor(255, 0, 0, 255);
                        auto drawlist = ImGui::GetOverlayDrawList();
                        drawlist->AddRect(ImVec2(agent_screen_pos[0] - width, mins[1]), ImVec2(agent_screen_pos[0] + width, maxs[1]), ImColor(boxColor), 0.0f, 0, 1);
                        drawlist->AddRect(ImVec2(agent_screen_pos[0] - width, maxs[1]), ImVec2(agent_screen_pos[0] + width, mins[1]), ImColor(boxColor), 0.0f, 0, 1);
                        float thickness = 1;
                        drawlist->AddLine(ImVec2(agent_screen_pos[0] - width, mins[1]), ImVec2(agent_screen_pos[0] + width, mins[1]), ImColor(0, 0, 0, 255), 1);
                        drawlist->AddLine(ImVec2(agent_screen_pos[0] - width, maxs[1]), ImVec2(agent_screen_pos[0] + width, maxs[1]), ImColor(0, 0, 0, 255), 1);
                        drawlist->AddLine(ImVec2(agent_screen_pos[0] - width, mins[1]), ImVec2(agent_screen_pos[0] - width, maxs[1]), ImColor(0, 0, 0, 255), 1);
                        drawlist->AddLine(ImVec2(agent_screen_pos[0] + width, mins[1]), ImVec2(agent_screen_pos[0] + width, maxs[1]), ImColor(0, 0, 0, 255), 1);
                    }
                    if (healthbar) {
                        const ImVec2 barMin = ImVec2(agent_screen_pos[0] - width - 5, mins[1]);
                        const ImVec2 barMax = ImVec2{ agent_screen_pos[0] - width - 2, maxs[1] };

                        const float green = (agent->health / agent->max_health);
                        const float greenClamped = (std::min)(255.f, green);
                        const float red = (std::min)(255.f, 1 - green);

                        float val = (agent->health / agent->max_health);
                        const float height = val * (barMin.y - barMax.y);
                        float h = val * (width * 4);
                        float barHeight = val * height;
                        auto drawlist = ImGui::GetOverlayDrawList();
                        drawlist->AddRectFilled(ImVec2(barMin.x, barMin.y - 1),
                            ImVec2(barMax.x, barMax.y),
                            IM_COL32(0, 0, 0, 255));
                        float lineWidth = max(width / 4, 1.f);
                        drawlist->AddRectFilled(
                            ImVec2{ barMin.x + 1, mins[1] - h },
                            ImVec2{ (barMax.x - 1), barMin.y - 1 },
                            IM_COL32((int)((1 - val) * 255), (int)(val * 255), 0, 255));
                    }
                    if (lines) {
                        D3DCOLOR color = D3DCOLOR_ARGB(255, 255, 0, 0);
                        DrawLine(windowWidth / 2, windowHeight, agent_screen_pos[0], mins[1], 1, color);
                    }
                }
            }
        }
    }
}
void visualHorseRender(bool esp, bool healthbar, bool lines) {
    DWORD cur_mission = 0x8b829c;
    size_t agentlist_size = *(size_t*)(*(uintptr_t*)cur_mission + 0x8);
    for (int i = 0; i < agentlist_size; i++) {
        Agent* agent = get_agent(i);
        if (!agent)
            continue;
        Agent* localAgent = getLocalAgent();
        if (localAgent && localAgent == agent)
            continue;
        D3DXVECTOR3 pos = agent->position;
        float agent_screen_pos[2] = { -1 };
        worldToScreen(pos, agent_screen_pos);

        if (agent_screen_pos[0] == -1) {
            continue;
        }
        if (agent->isHorse == 0) {
            continue;
        }
        rglStrategic_Entity* ent = agent->prgl_strat_ent;
        if (ent) {
            if (ent->ent_min && ent->ent_max && ent->ent_origin) {
                D3DXVECTOR3 min = ent->ent_min;
                D3DXVECTOR3 max = ent->ent_max;
                float mins[2] = { -1 };
                worldToScreen(min, mins);
                float maxs[2] = { -1 };
                worldToScreen(max, maxs);
                float width = fabs(mins[1] - maxs[1]) / 4;
                if (mins[0] != -1 && maxs[0] != -1) {
                    if (esp) {
                        ImVec4 boxColor(0, 0, 255, 255);
                        auto drawlist = ImGui::GetOverlayDrawList();
                        drawlist->AddRect(ImVec2(agent_screen_pos[0] - width, mins[1]), ImVec2(agent_screen_pos[0] + width, maxs[1]), ImColor(boxColor), 0.0f, 0, 1);
                        drawlist->AddRect(ImVec2(agent_screen_pos[0] - width, maxs[1]), ImVec2(agent_screen_pos[0] + width, mins[1]), ImColor(boxColor), 0.0f, 0, 1);
                        float thickness = 1;
                        drawlist->AddLine(ImVec2(agent_screen_pos[0] - width, mins[1]), ImVec2(agent_screen_pos[0] + width, mins[1]), ImColor(0, 0, 0, 255), 1);
                        drawlist->AddLine(ImVec2(agent_screen_pos[0] - width, maxs[1]), ImVec2(agent_screen_pos[0] + width, maxs[1]), ImColor(0, 0, 0, 255), 1);
                        drawlist->AddLine(ImVec2(agent_screen_pos[0] - width, mins[1]), ImVec2(agent_screen_pos[0] - width, maxs[1]), ImColor(0, 0, 0, 255), 1);
                        drawlist->AddLine(ImVec2(agent_screen_pos[0] + width, mins[1]), ImVec2(agent_screen_pos[0] + width, maxs[1]), ImColor(0, 0, 0, 255), 1);
                    }
                    if (healthbar) {
                        const ImVec2 barMin = ImVec2(agent_screen_pos[0] - width - 5, mins[1]);
                        const ImVec2 barMax = ImVec2{ agent_screen_pos[0] - width - 2, maxs[1] };

                        const float green = (agent->health / agent->max_health);
                        const float greenClamped = (std::min)(255.f, green);
                        const float red = (std::min)(255.f, 1 - green);

                        float val = (agent->health / agent->max_health);
                        const float height = val * (barMin.y - barMax.y);
                        float h = val * (width * 4);
                        float barHeight = val * height;
                        auto drawlist = ImGui::GetOverlayDrawList();
                        drawlist->AddRectFilled(ImVec2(barMin.x, barMin.y - 1),
                            ImVec2(barMax.x, barMax.y),
                            IM_COL32(0, 0, 0, 255));
                        float lineWidth = max(width / 4, 1.f);
                        drawlist->AddRectFilled(
                            ImVec2{ barMin.x + 1, mins[1] - h },
                            ImVec2{ (barMax.x - 1), barMin.y - 1 },
                            IM_COL32((int)((1 - val) * 255), (int)(val * 255), 0, 255));
                    }
                    if (lines) {
                        D3DCOLOR color = D3DCOLOR_ARGB(255, 0, 0, 255);
                        DrawLine(windowWidth / 2, windowHeight, agent_screen_pos[0], mins[1], 1, color);
                    }
                }
            }
        }
    }
}