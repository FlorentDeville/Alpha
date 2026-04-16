/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

namespace Systems
{
	class IClockSubsystem
	{
	public:
		virtual float GetTime() const = 0;
	};
}
