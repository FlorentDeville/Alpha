/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Systems/Game/GameContext.h"

#include "Systems/Game/Subsystems/CameraSubsystem.h"
#include "Systems/Game/Subsystems/Clock/IClockSubsystem.h"
#include "Systems/Particle/ParticleSystem.h"

namespace Systems
{
	World::World()
		: m_pParticleSystem(nullptr)
		, m_pCameraSubsystem(nullptr)
		, m_pClock(nullptr)
	{ }

	World::~World()
	{
		delete m_pParticleSystem;
		delete m_pCameraSubsystem;
		delete m_pClock;
	}
}
