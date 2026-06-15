/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Alpha/StateMachine/IState.h"
#include "Alpha/Objects/Boss/Ichi/Ichi.h"

class Ichi;
class IchiWaveP1A1;
class IchiWaveP2A1;

class Ichi_Phase3_Attack1 : public IState
{
public:
	Ichi_Phase3_Attack1(StateMachine* pStateMachine, Ichi* pIchi);
	~Ichi_Phase3_Attack1();

	void OnEnter() override;
	void OnUpdate() override;
	void OnExit() override;

	void InitWaves();
	void DestroyWaves();

private:
	enum InternalState
	{
		START_LOWER_TOWER_ODD_GUNS,
		LOWER_TOWER_ODD_GUNS,
		START_LOWER_TOWER_EVEN_GUNS,
		LOWER_TOWER_EVEN_GUNS,
		START_MIDDLE_TOWER,
		MIDDLE_TOWER,
		START_UPPER_TOWER,
		UPPER_TOWER,
		REST,

		COUNT
	};

	Ichi* m_pIchi;

	InternalState m_internalState;

	uint32_t m_lowerTowerWaveCount;
	IchiWaveP1A1** m_ppWaves;
	uint32_t* m_pWaveIndex;

	uint32_t m_middleTowerWaveCount;
	IchiWaveP2A1** m_ppMiddleTowerWaves;
	uint32_t* m_pMiddleTowerWaveIndex;

	uint32_t m_upperTowerWaveCount;
	IchiWaveP1A1** m_ppUpperTowerWaves;
	uint32_t* m_pUpperTowerWaveIndex;

	Systems::RenderableComponent* m_pLowerTowerRenderable;
	Systems::RenderableComponent* m_pMiddleTowerRenderable;
	Systems::RenderableComponent* m_pUpperTowerRenderable;

	float m_startTime;
	float m_startInternalStateTime;

	bool IsTimeElasped(float start, float current, float duration) const;

	void UpdateLowerTowerOddGuns();
	void UpdateLowerTowerEvenGuns();
	void UpdateMiddleTowerGuns();
	void UpdateUpperTowerWaves();

	void UpdateLowerTowerMotion(float rotationDirection);
};
