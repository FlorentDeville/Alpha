/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Alpha/StateMachine/IState.h"

class BossGameObject;

class BossState_Phase2_To_Phase3 : public IState
{
public:
	BossState_Phase2_To_Phase3(StateMachine* pStateMachine, BossGameObject* pBoss);
	~BossState_Phase2_To_Phase3();

	void OnEnter() override;
	void OnUpdate() override;
	void OnExit() override;

private:
	BossGameObject* m_pBoss;
};
