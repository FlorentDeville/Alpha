/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Core/Singleton.h"

#include <chrono>

namespace Systems
{
	//Global clock for the entire application. It never pauses nor stops.
	//Internally I use :
	// - system time : time returned by the system (os).
	// - application time : time elasped since the application started and calculated from the system time.
	class Clock : public Core::Singleton<Clock>
	{
	public:
		using Second = std::chrono::duration<float>;
		using TimePoint = std::chrono::steady_clock::time_point;

		Clock();
		~Clock();

		void Init() override;

		void Update();

		//Return the elasped time since the application started in seconds. 
		// This time is only updated once per frame so if you call it multiple time in the same frame, it returns the same result.
		float GetApplicationTime() const;

		//Return the real time elapsed time since the application started in seconds. It is not tied to a frame and compute
		// the real elasped time since the application started.
		float GetRealApplicationTime() const;

	private:
		TimePoint m_lastUpdate;			// time the last update of the clock happened

		Second m_applicationTime;	// time elapsed since the application started

		TimePoint GetSystemTime() const;
		Second ComputeApplicationTimeFromSystemTime(TimePoint now) const;
	};
}
