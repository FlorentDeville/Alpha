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
		, m_deltaTime(0)
		, m_isRunning(false)
		, m_timeScale(1)
	{ }

	GameClockSubsystem::~GameClockSubsystem()
	{ }

	void GameClockSubsystem::Update(float dt)
	{
		if (m_isRunning)
		{
			float scaledDt = dt * m_timeScale;
			m_elaspedTime += scaledDt;
			m_deltaTime = scaledDt;
		}
	}

	float GameClockSubsystem::GetTime() const
	{
		return m_elaspedTime;
	}

	float GameClockSubsystem::GetDeltaTime() const
	{
		return m_deltaTime;
	}

	void GameClockSubsystem::Start()
	{
		m_isRunning = true;
	}

	void GameClockSubsystem::Pause()
	{
		m_isRunning = false;
		m_deltaTime = 0;
	}

	void GameClockSubsystem::Stop()
	{
		m_isRunning = false;
		m_elaspedTime = 0;
		m_deltaTime = 0;
	}

	void GameClockSubsystem::SetTimeScale(float timeScale)
	{
		m_timeScale = timeScale;
	}
}
