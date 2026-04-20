/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Systems/Game/Subsystems/Clock/GameClockSubsystem.h"

#include "Systems/Clock/Clock.h"

namespace Systems
{
	GameClockSubsystem::GameClockSubsystem()
		: IClockSubsystem()
		, m_elaspedTime(0)
		, m_isRunning(false)
	{ }

	GameClockSubsystem::~GameClockSubsystem()
	{ }

	void GameClockSubsystem::Update(float dt)
	{
		if (m_isRunning)
			m_elaspedTime += dt;
	}

	float GameClockSubsystem::GetTime() const
	{
		return m_elaspedTime;
	}

	void GameClockSubsystem::Start()
	{
		m_isRunning = true;
	}

	void GameClockSubsystem::Pause()
	{
		m_isRunning = false;
	}

	void GameClockSubsystem::Stop()
	{
		m_isRunning = false;
		m_elaspedTime = 0;
	}
}
