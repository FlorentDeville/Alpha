/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Systems/Game/Subsystems/Clock/IClockSubsystem.h"

namespace Systems
{
	class GameClockSubsystem : public IClockSubsystem
	{
	public:
		GameClockSubsystem();
		~GameClockSubsystem();

		void Update(float dt) override;

		float GetTime() const override;
		float GetDeltaTime() const override;

		void Start() override;
		void Pause() override;
		void Stop() override;

		void SetTimeScale(float timeScale) override;

	private:
		float m_elaspedTime;
		float m_deltaTime;
		float m_timeScale;

		bool m_isRunning;
	};
}
