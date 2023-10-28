#pragma once
#include <d3dx9.h>
#define STR_MERGE_IMPL(a, b) a##b
#define STR_MERGE(a, b) STR_MERGE_IMPL(a, b)
#define MAKE_PAD(size) STR_MERGE(_pad, __COUNTER__)[size]
#define DEFINE_MEMBER_N(type, name, offset) struct {unsigned char MAKE_PAD(offset); type name;}
class Specifics {
public:
	union
	{
		DEFINE_MEMBER_N(int, is_moving, 0x24);
	};
};
class rglStrategic_Entity
{
public:
	union {
		DEFINE_MEMBER_N(D3DXVECTOR3, ent_min, 0x009C);
		DEFINE_MEMBER_N(D3DXVECTOR3, ent_max, 0x00AC);
		DEFINE_MEMBER_N(D3DXVECTOR3, ent_origin, 0x0088);
	};

};
class Agent
{
public:
	union {
		DEFINE_MEMBER_N(rglStrategic_Entity*, prgl_strat_ent, (DWORD)0x0868);
		DEFINE_MEMBER_N(D3DXVECTOR3, position, 0x0040);
		DEFINE_MEMBER_N(int, is_valid, 0x0004);
		DEFINE_MEMBER_N(float, health, 0x6000);
		DEFINE_MEMBER_N(float, max_health, 0x5FFC);
		DEFINE_MEMBER_N(Specifics, specifics, 0x02A8);
		DEFINE_MEMBER_N(int, isLocalAgent, 0x34);
		DEFINE_MEMBER_N(int, isHorse, 0x28);
		DEFINE_MEMBER_N(D3DXVECTOR3, bone_pos, 0x00B4);
	};
};
class Tactical_Window
{
public:
	union {
		DEFINE_MEMBER_N(void*, pScene_widget, (DWORD)0x0150);
	};
};


