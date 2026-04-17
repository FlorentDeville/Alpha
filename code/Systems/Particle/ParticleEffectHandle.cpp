/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Systems/Particle/ParticleEffectHandle.h"

namespace Systems
{
	ParticleEffectHandle::ParticleEffectHandle()
		: m_index(0)
		, m_generation(0)
	{ }

	bool ParticleEffectHandle::IsValid() const
	{
		return m_generation != 0;
	}
}
