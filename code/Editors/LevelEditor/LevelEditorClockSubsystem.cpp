/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Editors/LevelEditor/LevelEditorClockSubsystem.h"

#include "Systems/Clock/Clock.h"

namespace Editors
{
	LevelEditorClockSubsystem::LevelEditorClockSubsystem()
		: IClockSubsystem()
	{ }

	LevelEditorClockSubsystem::~LevelEditorClockSubsystem()
	{ }

	float LevelEditorClockSubsystem::GetTime() const
	{
		return Systems::Clock::Get().GetApplicationTime();
	}
}
