/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Alpha/StateMachine/IState.h"
#include "Alpha/Objects/Boss/Ichi/Ichi.h"

class Ichi;

class Ichi_Phase1_To_Phase2 : public IState
{
public:
	Ichi_Phase1_To_Phase2(StateMachine* pStateMachine, Ichi* pIchi);
	~Ichi_Phase1_To_Phase2();

	void OnEnter() override;
	void OnUpdate() override;
	void OnExit() override;

private:
	enum InternalState
	{
		REST_POSITION,
		DELAY,
		RUMBLE,
		SMOKE,
		STOP_RUMBLE,
		DELAY2
	};

	Ichi* m_pIchi;

	InternalState m_internalState;
	float m_internalStateStartTime;

	Core::Vec4f m_rumbleStateInitialPosition;
	float m_rumbleStartTime;

	Core::Quaternion m_initialOrientation;

	bool ElapsedTime(float start, float duration) const;

	void Rumble();
};
