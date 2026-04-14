/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Core/Math/Mat44f.h"
#include "Core/Math/Vec4f.h"

#include "Rendering/BufferHandle.h"

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

		void Init(uint32_t spawnRate, float lifetime, const Core::Vec4f& acceleration, const Core::Mat44f& transform);

		void Update(float dtInSeconds);

		void Upload();

	private:
		Particles m_particles;
		Core::Mat44f m_transform;
		Core::Vec4f m_acceleration;
		int m_spawnRate;
		float m_lifetime;

		//rendering
		Rendering::BufferHandle m_gfxBufferPositions;
		void* m_gfxBufferPositionPtr;

		void KillParticle(int index);
		void SpawnParticle();
	};
}
