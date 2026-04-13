/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Core/Reflection/ReflectionMacro.h"

ENABLE_REFLECTION(Systems, ParticleEmitter)

namespace Systems
{
	class ParticleEmitter
	{
	public:
		ParticleEmitter() = default;

	private:
		START_REFLECTION(Systems::ParticleEmitter)
		END_REFLECTION()
	};
}
