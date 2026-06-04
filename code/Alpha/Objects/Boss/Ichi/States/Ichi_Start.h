/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Alpha/StateMachine/IState.h"

class Ichi_Start : public IState
{
public:
	Ichi_Start(StateMachine* pStateMachine);
	~Ichi_Start() = default;

	void OnEnter() override;
	void OnUpdate() override;
	void OnExit() override;
};
