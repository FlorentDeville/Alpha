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
		TimePoint now = GetSystemTime();
		m_applicationTime = ComputeApplicationTimeFromSystemTime(now);
		m_lastUpdate = now;
	}

	float Clock::GetApplicationTime() const
	{
		return m_applicationTime.count();
	}

	float Clock::GetRealApplicationTime() const
	{
		Second realTime = ComputeApplicationTimeFromSystemTime(GetSystemTime());
		return realTime.count();

	}

	Clock::TimePoint Clock::GetSystemTime() const
	{
		TimePoint now = std::chrono::steady_clock::now();
		return now;
	}

	Clock::Second Clock::ComputeApplicationTimeFromSystemTime(TimePoint now) const
	{
		std::chrono::steady_clock::duration dt = now - m_lastUpdate;
		Second dtInSeconds = std::chrono::duration_cast<Second>(dt);

		Second realTime = m_applicationTime + dtInSeconds;
		return realTime;
	}
}
