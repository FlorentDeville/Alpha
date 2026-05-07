/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include <cstdint>

class StateMachine;

class IState
{
public:
	IState(StateMachine* pStateMachine);
	virtual ~IState();

	virtual void OnEnter();
	virtual void OnUpdate();
	virtual void OnExit();

	void GoTo(uint32_t newState);

private:
	StateMachine* m_pStateMachine;
};