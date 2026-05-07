/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Alpha/StateMachine/IState.h"

class BossStateWaveTest : public IState
{
public:
	BossStateWaveTest();
	~BossStateWaveTest();

	void OnEnter() override;
	void OnUpdate() override;
	void OnExit() override;
};
