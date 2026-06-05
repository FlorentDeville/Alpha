/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Core/Collections/Array.h"

class IState;

class StateMachine
{
public:
	StateMachine();
	~StateMachine();

	void Init(uint32_t stateCount);
	void AddState(IState* pState, uint32_t index);

	void Start(uint32_t state);
	void Update();

	void GoTo(uint32_t newState);

	uint32_t GetCurrentState() const;

	template<typename T> T* GetState(uint32_t stateIndex);

private:
	Core::Array<IState*> m_states;

	uint32_t m_currentState;
	uint32_t m_nextState;
};

template<typename T> T* StateMachine::GetState(uint32_t stateIndex)
{
	return static_cast<T*>(m_states[stateIndex]);
}
