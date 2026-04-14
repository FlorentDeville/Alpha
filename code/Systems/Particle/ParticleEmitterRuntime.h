/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Core/Math/Sqt.h"

namespace Core
{
	class Vec4f;
}

namespace Systems
{
	class ParticleEmitterRuntime
	{
	public:

		class Particles
		{
		public:
			Particles() = default;

			Core::Vec4f* m_position;
			Core::Vec4f* m_velocity;
			float* m_timeToDeath;

			int m_maxCount;
			int m_currentCount;
		};

		ParticleEmitterRuntime();
		~ParticleEmitterRuntime();

		void Init(int spawnRate, float lifetime, const Core::Vec4f& acceleration, const Core::Sqt& transform);

		void Update(float dtInSeconds);

	private:
		Particles m_particles;
		Core::Sqt m_transform;
		Core::Vec4f m_acceleration;
		int m_spawnRate;
		float m_lifetime;

		void KillParticle(int index);
		void SpawnParticle();
	};
}
