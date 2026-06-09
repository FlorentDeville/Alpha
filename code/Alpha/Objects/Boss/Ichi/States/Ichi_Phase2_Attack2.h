/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Alpha/StateMachine/IState.h"
#include "Alpha/Objects/Boss/Ichi/Ichi.h"

class Ichi;
class IchiWaveP1A2MainBeam;
class IchiWaveP1A2SideBeam;
class IchiWaveP1A2BackBeam;

class Ichi_Phase2_Attack2 : public IState
{
public:
	Ichi_Phase2_Attack2(StateMachine* pStateMachine, Ichi* pIchi);
	~Ichi_Phase2_Attack2();

	void OnEnter() override;
	void OnUpdate() override;
	void OnExit() override;

	void InitWaves();
	void DestroyWaves();

private:
	Ichi* m_pIchi;

	static const uint8_t MAIN_BEAM_COUNT = 2;
	IchiWaveP1A2MainBeam* m_pMainBeam[MAIN_BEAM_COUNT];
	uint32_t m_mainBeamIndex[MAIN_BEAM_COUNT];

	static const uint8_t SIDE_BEAM_COUNT = 4;
	IchiWaveP1A2SideBeam* m_pSideBeam[SIDE_BEAM_COUNT];
	uint32_t m_sideBeamIndex[SIDE_BEAM_COUNT];

	static const uint8_t BACK_BEAM_COUNT = 2;
	IchiWaveP1A2BackBeam* m_pBackBeam[BACK_BEAM_COUNT];
	uint32_t m_backBeamIndex[BACK_BEAM_COUNT];

	Core::Vec4f m_waypoints[2];
	uint8_t m_currentWaypointIndex;
};
