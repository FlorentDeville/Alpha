/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Alpha/StateMachine/IState.h"
#include "Alpha/Objects/Boss/Ichi/Ichi.h"

class Ichi;

class Ichi_Phase2_To_Phase3 : public IState
{
public:
	Ichi_Phase2_To_Phase3(StateMachine* pStateMachine, Ichi* pIchi);
	~Ichi_Phase2_To_Phase3();

	void OnEnter() override;
	void OnUpdate() override;
	void OnExit() override;

private:
	Ichi* m_pIchi;
};
