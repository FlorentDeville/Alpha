/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Alpha/StateMachine/IState.h"

#include "Core/Math/Vec4f.h"

class Ichi;

class IchiMotionStrafe : public IState
{
public:
	IchiMotionStrafe(StateMachine* pMachine, Ichi* pIchi);
	~IchiMotionStrafe() = default;

	void OnEnter() override;
	void OnUpdate() override;
	void OnExit() override;

	void SetTarget(const Core::Vec4f& target);
	void SetSpeed(float speed);

private:
	Ichi* m_pIchi;

	Core::Vec4f m_target;
	float m_speed;
};
