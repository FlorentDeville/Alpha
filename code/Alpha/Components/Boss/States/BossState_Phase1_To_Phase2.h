/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Alpha/StateMachine/IState.h"

class BossGameObject;

class BossState_Phase1_To_Phase2 : public IState
{
public:
	BossState_Phase1_To_Phase2(StateMachine* pStateMachine, BossGameObject* pBoss);
	~BossState_Phase1_To_Phase2();

	void OnEnter() override;
	void OnUpdate() override;
	void OnExit() override;

private:
	BossGameObject* m_pBoss;
};
