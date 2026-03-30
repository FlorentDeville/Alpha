/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Systems/Clock/Clock.h"

namespace Systems
{
	Clock::Clock()
		: Core::Singleton<Clock>()
		, m_isGameClockRunning(false)
		, m_applicationTime()
		, m_gameTime()
		, m_lastUpdate()
	{ }

	Clock::~Clock()
	{ }

	void Clock::Init()
	{
		m_applicationTime = Second(0);
		m_gameTime = Second(0);
		
		m_lastUpdate = std::chrono::steady_clock::now();
	}

	void Clock::Update()
	{
		std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
		std::chrono::steady_clock::duration dt = now - m_lastUpdate;
		Second dtInSeconds = std::chrono::duration_cast<Second>(dt);

		m_applicationTime += dtInSeconds;

		if(m_isGameClockRunning)
			m_gameTime += dtInSeconds;

		m_lastUpdate = now;
	}

	void Clock::StartGameClock()
	{
		m_isGameClockRunning = true;
	}

	void Clock::StopGameClock()
	{
		m_isGameClockRunning = false;
		m_gameTime = Second(0);
	}

	void Clock::PauseGameClock()
	{
		m_isGameClockRunning = false;
	}

	//Return the elasped time since the application started in seconds.
	float Clock::GetApplicationTime() const
	{
		return m_applicationTime.count();
	}

	//Return the elapsed time since the game started in seconds.
	float Clock::GetGameTime() const
	{
		return m_gameTime.count();
	}
}
