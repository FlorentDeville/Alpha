/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Alpha/StateMachine/IState.h"
#include "Alpha/Objects/Boss/Ichi/Ichi.h"

class Ichi;
class IchiWaveP1A1;
class IchiWaveP2A1;

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
	IchiWaveP2A1** m_pWave;
	uint32_t* m_waveIndex;

	static const uint8_t UPPER_GUNS_COUNT = 4;
	IchiWaveP1A1* m_pUpperGunsWaves[UPPER_GUNS_COUNT];
	uint32_t m_upperGunsWaveIndex[UPPER_GUNS_COUNT];

	float m_startTime;

	Systems::RenderableComponent* m_pP1Renderable;
	Systems::RenderableComponent* m_pP2Renderable;

	void UpdateWavesSpawnParameters();

	void UpdateMotion();

	void UpdateUpperGunWavesParameters();
	void UpdateMiddleGunWavesParameters();
};
