/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Systems/Clock/Clock.h"

namespace Systems
{
	Clock::Clock()
		: Core::Singleton<Clock>()
		, m_applicationTime()
		, m_lastUpdate()
	{ }

	Clock::~Clock()
	{ }

	void Clock::Init()
	{
		m_applicationTime = Second(0);
		
		m_lastUpdate = std::chrono::steady_clock::now();
	}

	void Clock::Update()
	{
		std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
		std::chrono::steady_clock::duration dt = now - m_lastUpdate;
		Second dtInSeconds = std::chrono::duration_cast<Second>(dt);

		m_applicationTime += dtInSeconds;

		m_lastUpdate = now;
	}

	//Return the elasped time since the application started in seconds.
	float Clock::GetApplicationTime() const
	{
		return m_applicationTime.count();
	}
}
