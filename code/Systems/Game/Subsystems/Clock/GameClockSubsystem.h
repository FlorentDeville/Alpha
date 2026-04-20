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

		void Start() override;
		void Pause() override;
		void Stop() override;

	private:
		float m_elaspedTime;
		bool m_isRunning;;
	};
}
