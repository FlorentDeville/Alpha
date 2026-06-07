/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Alpha/StateMachine/IState.h"
#include "Alpha/Objects/Boss/Ichi/Ichi.h"

class Ichi;
class IchiWaveP1A1;

class Ichi_Phase1_Attack1 : public IState
{
public:
	Ichi_Phase1_Attack1(StateMachine* pStateMachine, Ichi* pIchi);
	~Ichi_Phase1_Attack1();

	void OnEnter() override;
	void OnUpdate() override;
	void OnExit() override;

private:
	Ichi* m_pIchi;

	static const uint8_t WAVE_COUNT = 4;
	IchiWaveP1A1* m_pWave[WAVE_COUNT];
	uint32_t m_waveIndex[WAVE_COUNT];

	float m_rotationSpeed;

	float m_warmupDuration; //time to wait before turning
	float m_warmupStartTime;

	void UpdateWaves();
	
};
