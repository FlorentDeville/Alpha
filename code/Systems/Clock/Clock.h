/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Core/Singleton.h"

#include <chrono>

namespace Systems
{
	//Definition of the clocks:
	// Application : time since the application started.
	// Game : time since the game started. It can be different from the application time when running the editor.
	class Clock : public Core::Singleton<Clock>
	{
	public:
		using Second = std::chrono::duration<float>;

		Clock();
		~Clock();

		void Init() override;

		void Update();

		void StartGameClock();
		void StopGameClock();
		void PauseGameClock();

		//Return the elasped time since the application started in seconds.
		float GetApplicationTime() const;

		//Return the elapsed time since the game started in seconds.
		float GetGameTime() const;

	private:
		std::chrono::steady_clock::time_point m_lastUpdate;			// time the last update of the clock happened

		Second m_applicationClock;	// time elapsed since the application started
		
		Second m_gameClock;			// time elapsed since the game started
		bool m_isGameClockRunning;
	};
}
