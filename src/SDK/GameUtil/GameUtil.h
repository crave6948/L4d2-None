#pragma once

#include "../KeyValues/KeyValues.h"

class CGlobal_GameUtil
{
public:
	void FixMovement(const Vector vAngle, CUserCmd* cmd);
	bool IsVisible(Vector start, Vector end, ITraceFilter* filter);
	bool IsHitEntity(Vector start, Vector end, C_BaseEntity* pEntity, ITraceFilter* filter);
	C_BaseEntity* GetHitEntity(Vector start, Vector end, ITraceFilter* filter);
	void Trace(const Vector& start, const Vector& end, unsigned int mask, ITraceFilter* filter, trace_t* trace);

	bool W2S(const Vector vWorld, Vector& vScreen);
	bool IsOnScreen(const Vector vWorld);
	bool IsValidTeam(const int nTeam);
	bool IsInfectedAlive(const int nSolidFlags, const int nSequence);

	Color GetHealthColor(const int nHealth, const int nMaxHealth);

	IMaterial* CreateMaterial(const char* const szVars);
};

namespace G { inline CGlobal_GameUtil Util; }