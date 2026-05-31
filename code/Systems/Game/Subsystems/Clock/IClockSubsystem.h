/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Systems/Game/Subsystems/ISubsystem.h"

namespace Systems
{
	class IClockSubsystem : public ISubsystem
	{
	public:

		virtual void Update(float dt) = 0;

		virtual float GetTime() const = 0;
		virtual float GetDeltaTime() const = 0;

		virtual void Start() = 0;
		virtual void Pause() = 0;
		virtual void Stop() = 0;
	};
}
