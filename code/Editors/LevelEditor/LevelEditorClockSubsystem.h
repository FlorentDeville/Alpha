/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Systems/Game/Subsystems/Clock/IClockSubsystem.h"

namespace Editors
{
	class LevelEditorClockSubsystem : public Systems::IClockSubsystem
	{
	public:
		LevelEditorClockSubsystem();
		~LevelEditorClockSubsystem();

		float GetTime() const override;
	};
}
