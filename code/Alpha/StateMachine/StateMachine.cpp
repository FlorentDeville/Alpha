/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Alpha/StateMachine/StateMachine.h"

#include "Alpha/StateMachine/IState.h"

StateMachine::StateMachine()
	: m_states()
	, m_currentState(0)
	, m_nextState(0)
{ }

StateMachine::~StateMachine()
{
	for (IState* pState : m_states)
		delete pState;
}

void StateMachine::Init(uint32_t stateCount)
{
	m_states.Resize(stateCount, nullptr);
}

void StateMachine::AddState(IState* pState, uint32_t index)
{
	m_states[index] = pState;
}

void StateMachine::Start(uint32_t state)
{
	m_currentState = UINT32_MAX;
	m_nextState = state;
}

void StateMachine::Update()
{
	if (m_currentState != m_nextState)
	{
		if (m_states.IsValidIndex(m_currentState))
			m_states[m_currentState]->OnExit();

		m_states[m_nextState]->OnEnter();

		m_currentState = m_nextState;
		return; //don't enter and update in the same frame
	}

	m_states[m_currentState]->OnUpdate();
}

void StateMachine::GoTo(uint32_t newState)
{
	m_nextState = newState;
}

uint32_t StateMachine::GetCurrentState() const
{
	return m_currentState;
}
