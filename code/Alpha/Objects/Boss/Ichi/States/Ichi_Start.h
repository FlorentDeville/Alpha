/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Alpha/StateMachine/IState.h"
#include "Alpha/Objects/Boss/Ichi/Ichi.h"

class Ichi_Start : public IState
{
public:
	Ichi_Start(StateMachine* pStateMachine, Ichi* pIchi);
	~Ichi_Start() = default;

	void OnEnter() override;
	void OnUpdate() override;
	void OnExit() override;

	void Skip();

private:
	enum State
	{
		WAIT,		// wait a small amount of time before doing anything
		RUMBLE,		// rumble before engine ignition
		LIFTOFF,	// engine ignition and lift off the ground
		OVER,		// wait a small amount of time before leaving the state

		COUNT
	};

	class ParameterStateWait
	{
	public:
		float m_duration;
	};

	class ParameterStateRumble
	{
	public:
		float m_duration;
		float m_intensity;
		float m_frequency;
		Core::Vec4f m_initialPosition;
	};

	class ParameterStateLiftoff
	{
	public:
		float m_duration;
		float m_distance;
		Core::Vec4f m_initialPosition;
	};

	class ParameterStateOver
	{
	public:
		float m_duration;
	};

	Ichi* m_pIchi;

	State m_state;

	ParameterStateWait m_stateWaitParam;
	ParameterStateRumble m_stateRumbleParam;
	ParameterStateLiftoff m_stateLiftoffParam;
	ParameterStateOver m_stateOverParam;

	float m_stateStartTime;

	void Update_Wait();
	void Update_Rumble();

	void Enter_Liftoff();
	void Update_Liftoff();
	
	void Enter_Over();
	void Update_Over();
};
