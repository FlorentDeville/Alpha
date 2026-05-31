/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include <cstdint>

namespace Systems
{
	class ParticleEffectHandle
	{
	public:
		ParticleEffectHandle();

		bool IsValid() const;

		uint32_t m_index;
		uint32_t m_generation;
	};
}
