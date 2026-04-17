/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Editors/LevelEditor/LevelEditorClockSubsystem.h"

#include "Systems/Clock/Clock.h"

namespace Editors
{
	LevelEditorClockSubsystem::LevelEditorClockSubsystem()
		: IClockSubsystem()
		, m_currentTime(0)
	{ }

	LevelEditorClockSubsystem::~LevelEditorClockSubsystem()
	{ }

	float LevelEditorClockSubsystem::GetTime() const
	{
		return m_currentTime;
	}

	void LevelEditorClockSubsystem::Update(float dt)
	{
		m_currentTime += dt;
	}
}
