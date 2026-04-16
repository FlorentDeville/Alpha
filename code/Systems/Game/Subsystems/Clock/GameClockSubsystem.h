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

		float GetTime() const override;
	};
}
