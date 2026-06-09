/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Alpha/StateMachine/IState.h"
#include "Alpha/Objects/Boss/Ichi/Ichi.h"

class Ichi;
class IchiWaveP1A1;

class Ichi_Phase2_Attack1 : public IState
{
public:
	Ichi_Phase2_Attack1(StateMachine* pStateMachine, Ichi* pIchi);
	~Ichi_Phase2_Attack1();

	void OnEnter() override;
	void OnUpdate() override;
	void OnExit() override;

	void InitWaves();
	void DestroyWaves();

private:
	enum InternalState
	{
		WARMUP,
		SHOOT,
		FINISH
	};

	InternalState m_internalState;

	Ichi* m_pIchi;

	uint8_t m_waveCount;
	IchiWaveP1A1** m_pWave;
	uint32_t* m_waveIndex;

	float m_startTime;

	void UpdateWavesSpawnParameters();

	void UpdateMotion();
};
