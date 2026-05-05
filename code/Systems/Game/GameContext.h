/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

namespace Systems
{
	class CameraSubsystem;
	class IClockSubsystem;
	class ParticleSystem;

	//This represent an instance of all the subsystems used by the game. We can have multiple worlds at the same time
	//for example one for the game and one for the level editor.
	//The world owns the systems so it deletes them in the destructor
	class World
	{
	public:
		World();
		~World();

		CameraSubsystem* m_pCameraSubsystem;
		ParticleSystem* m_pParticleSystem;
		IClockSubsystem* m_pClock;
	};
}
