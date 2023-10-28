#pragma once
#include "Cheat.h"
void Init();
Agent* get_agent(int id);
Agent* getLocalAgent();
void visualTeamRender(bool esp, bool healthbar, bool lines);
void visualHorseRender(bool esp, bool healthbar, bool lines);
void visualEnemyRender(bool esp, bool healthbar, bool lines);
void someTest();