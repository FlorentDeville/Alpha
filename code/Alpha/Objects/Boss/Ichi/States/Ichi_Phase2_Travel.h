/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Alpha/StateMachine/IState.h"
#include "Alpha/Objects/Boss/Ichi/Ichi.h"

class Ichi;

class Ichi_Phase2_Travel : public IState
{
public:
	Ichi_Phase2_Travel(StateMachine* pStateMachine, Ichi* pIchi);
	~Ichi_Phase2_Travel();

	void OnEnter() override;
	void OnUpdate() override;
	void OnExit() override;

private:
	Ichi* m_pIchi;

	static const uint8_t WAYPOINT_COUNT = 2;
	Core::Vec4f m_waypoint[WAYPOINT_COUNT];

	uint8_t m_currentWaypoint;

	bool m_nextAttack;
};
