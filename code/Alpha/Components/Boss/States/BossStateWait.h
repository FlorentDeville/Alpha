/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Alpha/StateMachine/IState.h"

class BossStateWait : public IState
{
public:
	BossStateWait(StateMachine* pStateMachine);
	~BossStateWait();

	void OnEnter() override;
	void OnUpdate() override;
	void OnExit() override;

private:
	float m_startTime;
	float m_waitTime;
};
