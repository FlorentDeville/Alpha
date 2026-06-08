/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Alpha/StateMachine/IState.h"
#include "Alpha/Objects/Boss/Ichi/Ichi.h"

class Ichi;
class IchiWaveP1A2BackBeam;
class IchiWaveP1A2MainBeam;
class IchiWaveP1A2SideBeam;

class Ichi_Phase1_Attack2 : public IState
{
public:
	Ichi_Phase1_Attack2(StateMachine* pStateMachine, Ichi* pIchi);
	~Ichi_Phase1_Attack2();

	void OnEnter() override;
	void OnUpdate() override;
	void OnExit() override;

	void InitWaves();
	void DestroyWaves();

private:
	Ichi* m_pIchi;

	IchiWaveP1A2MainBeam* m_pMainBeam;
	uint32_t m_mainBeamIndex;

	static const uint8_t SIDE_BEAM_COUNT = 2;
	IchiWaveP1A2SideBeam* m_pSideBeam[SIDE_BEAM_COUNT];
	uint32_t m_sideBeamIndex[SIDE_BEAM_COUNT];

	IchiWaveP1A2BackBeam* m_pBackBeam;
	uint32_t m_backBeamIndex;

	Core::Vec4f m_waypoints[2];
	uint8_t m_currentWaypointIndex;
};
