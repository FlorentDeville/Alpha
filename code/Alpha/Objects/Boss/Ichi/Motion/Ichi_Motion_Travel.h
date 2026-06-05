/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Alpha/StateMachine/IState.h"

#include "Core/Math/Vec4f.h"

class Ichi;

class IchiMotionTravel : public IState
{
public:
	IchiMotionTravel(StateMachine* pMachine, Ichi* pIchi);
	~IchiMotionTravel() = default;

	void OnEnter() override;
	void OnUpdate() override;
	void OnExit() override;

	void SetTarget(const Core::Vec4f& target);

private:
	Ichi* m_pIchi;

	Core::Vec4f m_target;

	float m_length2;

	float m_speed;

	float m_distanceAcc;
	float m_distanceMaxSpeed;
	float m_distanceDec;
};
