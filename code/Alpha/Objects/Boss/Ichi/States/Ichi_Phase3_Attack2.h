/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Alpha/StateMachine/IState.h"
#include "Alpha/Objects/Boss/Ichi/Ichi.h"

class Ichi;
class IchiWaveP1A2BackBeam;
class IchiWaveP1A2MainBeam;
class IchiWaveP3A2;

class Ichi_Phase3_Attack2 : public IState
{
public:
	Ichi_Phase3_Attack2(StateMachine* pStateMachine, Ichi* pIchi);
	~Ichi_Phase3_Attack2();

	void OnEnter() override;
	void OnUpdate() override;
	void OnExit() override;

	void InitWaves();
	void DestroyWaves();

private:
	enum InternalState
	{
		UPPER_TOWER_WAVES,
		MIDDLE_TOWER_WAVES,
		LOWER_TOWER_WAVES,

		COUNT
	};

	Ichi* m_pIchi;

	InternalState m_internalState;
	float m_internaStateStartTime;

	static const uint8_t MAIN_BEAM_COUNT = 2;
	IchiWaveP1A2MainBeam* m_pMainBeam[MAIN_BEAM_COUNT];
	uint32_t m_mainBeamIndex[MAIN_BEAM_COUNT];

	IchiWaveP3A2* m_pLowerWave;
	uint32_t m_lowerWaveIndex;

	IchiWaveP1A2BackBeam* m_pBackBeam;
	uint32_t m_backBeamIndex;

	Core::Vec4f m_waypoints[2];
	uint8_t m_currentWaypointIndex;

	void GoToInternalStateUpperTowerWaves();
	void GoToInternalStateLowerTowerWaves();

	void UpdateLowerTowerWaves();
	void RotateLowerTower(float speed);
};
