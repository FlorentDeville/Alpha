/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Systems/Game/GameContext.h"

#include "Systems/Game/Subsystems/Camera/CameraSubsystem.h"
#include "Systems/Game/Subsystems/Clock/IClockSubsystem.h"
#include "Systems/Particle/ParticleSystem.h"

namespace Systems
{
	GameContext::GameContext()
		: m_pParticleSystem(nullptr)
		, m_pCameraSubsystem(nullptr)
		, m_pClock(nullptr)
	{ }

	GameContext::~GameContext()
	{
		delete m_pParticleSystem;
		delete m_pCameraSubsystem;
		delete m_pClock;
	}
}
