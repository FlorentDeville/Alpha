/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Systems/Game/Subsystems/Clock/GameClockSubsystem.h"

#include "Systems/Clock/Clock.h"

namespace Systems
{
	GameClockSubsystem::GameClockSubsystem()
		: IClockSubsystem()
	{ }

	GameClockSubsystem::~GameClockSubsystem()
	{ }

	float GameClockSubsystem::GetTime() const
	{
		return Systems::Clock::Get().GetGameTime();
	}
}
