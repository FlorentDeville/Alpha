/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Alpha/StateMachine/IState.h"

class BossGameObject;

class BossState_Start : public IState
{
public:
	BossState_Start(StateMachine* pStateMachine, BossGameObject* pBoss);
	~BossState_Start();

	void OnEnter() override;
	void OnUpdate() override;
	void OnExit() override;

private:
	BossGameObject* m_pBoss;

	float m_startTime;
};
