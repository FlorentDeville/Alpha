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

	void* GetContext();
	const void* GetContext() const;

	template<typename T> T* GetContext();
	template<typename T> const T* GetContext() const;

private:
	StateMachine* m_pStateMachine;
};

template<typename T> T* IState::GetContext()
{
	void* pRes = GetContext();
	return static_cast<T*>(pRes);
}

template<typename T> const T* IState::GetContext() const
{
	const void* pRes = GetContext();
	return static_cast<const T*>(pRes);
}
