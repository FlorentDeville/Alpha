/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Alpha/StateMachine/IState.h"

#include "Alpha/StateMachine/StateMachine.h"

IState::IState(StateMachine* pStateMachine)
	: m_pStateMachine(pStateMachine)
{ }

IState::~IState()
{ }

void IState::OnEnter()
{ }

void IState::OnUpdate()
{ }

void IState::OnExit()
{ }

void IState::GoTo(uint32_t newState)
{
	m_pStateMachine->GoTo(newState);
}

void* IState::GetContext()
{
	return m_pStateMachine->GetContext();
}

const void* IState::GetContext() const
{
	return m_pStateMachine->GetContext();
}
