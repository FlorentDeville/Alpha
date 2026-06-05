/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Alpha/StateMachine/IState.h"

#include "Core/Math/Vec4f.h"

class Ichi;

class IchiMotionStop : public IState
{
public:
	IchiMotionStop(StateMachine* pMachine, Ichi* pIchi);
	~IchiMotionStop() = default;

	void OnEnter() override;
	void OnUpdate() override;
	void OnExit() override;

private:
	Ichi* m_pIchi;
};
