/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Alpha/StateMachine/IState.h"
#include "Alpha/Objects/Boss/Ichi/Ichi.h"

class Ichi;

class Ichi_Phase1_Travel : public IState
{
public:
	Ichi_Phase1_Travel(StateMachine* pStateMachine, Ichi* pIchi);
	~Ichi_Phase1_Travel() = default;

	void OnEnter() override;
	void OnUpdate() override;
	void OnExit() override;

private:
	Ichi* m_pIchi;

	static const uint8_t TARGET_COUNT = 2;
	Core::Vec4f m_target[TARGET_COUNT];

	uint8_t m_currentTarget;

	bool m_nextAttack;
	bool m_targetInitialized;
};
