/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Alpha/StateMachine/IState.h"

#include "Core/Math/Vec4f.h"

class Ichi;

class IchiMotionIdle : public IState
{
public:
	IchiMotionIdle(StateMachine* pMachine, Ichi* pIchi);
	~IchiMotionIdle() = default;

	void OnEnter() override;
	void OnUpdate() override;
	void OnExit() override;

private:
	Ichi* m_pIchi;

	Core::Vec4f m_initialPosition;
	float m_amplitude;
	float m_frequency;
	float m_startTime;

	void ComputeBounce();
};
