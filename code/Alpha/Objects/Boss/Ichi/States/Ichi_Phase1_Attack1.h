/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Alpha/StateMachine/IState.h"
#include "Alpha/Objects/Boss/Ichi/Ichi.h"

class Ichi;

class Ichi_Phase1_Attack1 : public IState
{
public:
	Ichi_Phase1_Attack1(StateMachine* pStateMachine, Ichi* pIchi);
	~Ichi_Phase1_Attack1() = default;

	void OnEnter() override;
	void OnUpdate() override;
	void OnExit() override;

private:
	Ichi* m_pIchi;

	float m_currentRotation;
	float m_rotationSpeed;
};
