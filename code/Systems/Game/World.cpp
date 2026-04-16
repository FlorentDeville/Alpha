/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Systems/Game/World.h"

#include "Systems/Particle/ParticleSystem.h"

namespace Systems
{
	World::World()
		: m_pParticleSystem(nullptr)
	{ }

	World::~World()
	{
		delete m_pParticleSystem;
	}
}
