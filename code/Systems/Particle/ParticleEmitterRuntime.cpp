/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Systems/Particle/ParticleEmitterRuntime.h"

#include <cmath>
#include <random>

#include "Rendering/RenderModule.h"

namespace Systems
{
	ParticleEmitterRuntime::ParticleEmitterRuntime()
		: m_spawnRate()
		, m_lifetime()
		, m_acceleration()
		, m_particles()
	{ }

	ParticleEmitterRuntime::~ParticleEmitterRuntime()
	{
		delete[] m_particles.m_position;
		delete[] m_particles.m_timeToDeath;
		delete[] m_particles.m_velocity;

		Rendering::RenderModule::Get().DeleteBuffer(m_gfxBufferPositions);
	}

	void ParticleEmitterRuntime::Init(int spawnRate, float lifetime, const Core::Vec4f& acceleration, const Core::Sqt& transform)
	{
		m_spawnRate = spawnRate;
		m_lifetime = lifetime;
		m_acceleration = acceleration;
		m_transform = transform;

		int iLifetime = static_cast<int>(std::ceil(lifetime));
		m_particles.m_maxCount = spawnRate * iLifetime;

		m_particles.m_position = new Core::Vec4f[m_particles.m_maxCount];
		m_particles.m_velocity = new Core::Vec4f[m_particles.m_maxCount];
		m_particles.m_timeToDeath = new float[m_particles.m_maxCount];

		m_gfxBufferPositions = Rendering::RenderModule::Get().CreateBufferForParticles(sizeof(Core::Vec4f), m_particles.m_maxCount);
		m_gfxBufferPositionPtr = Rendering::RenderModule::Get().MapBuffer(m_gfxBufferPositions);
	}

	void ParticleEmitterRuntime::Update(float dtInSeconds)
	{
		//first spawn new particles
		int particlesToSpawn = static_cast<int>(m_spawnRate * dtInSeconds);

		for (int ii = 0; ii < particlesToSpawn; ++ii)
		{
			assert(m_particles.m_currentCount < m_particles.m_maxCount);
			SpawnParticle();
		}

		//kill dead particles particles
		for (int ii = 0; ii < m_particles.m_currentCount; ++ii)
		{
			m_particles.m_timeToDeath[m_particles.m_currentCount] -= dtInSeconds;
			if (m_particles.m_timeToDeath[m_particles.m_currentCount] <= 0)
			{
				KillParticle(ii);

				// KillParticle will put the last particle in place of the deleted particle. So now index ii points to a particle not checked yet.
				--ii;
			}
		}

		//update velocity
		for (int ii = 0; ii < m_particles.m_currentCount; ++ii)
			m_particles.m_velocity[ii] = m_particles.m_velocity[ii] + (m_acceleration * dtInSeconds);
		
		//update position
		for (int ii = 0; ii < m_particles.m_currentCount; ++ii)
			m_particles.m_position[ii] = m_particles.m_position[ii] + (m_particles.m_velocity[ii] * dtInSeconds);
	}

	void ParticleEmitterRuntime::Upload()
	{
		memcpy(m_gfxBufferPositionPtr, m_particles.m_position, sizeof(m_particles.m_position[0]) * m_particles.m_currentCount);
	}

	void ParticleEmitterRuntime::KillParticle(int index)
	{
		m_particles.m_position[index] = m_particles.m_position[m_particles.m_currentCount];
		m_particles.m_velocity[index] = m_particles.m_velocity[m_particles.m_currentCount];
		m_particles.m_timeToDeath[index] = m_particles.m_timeToDeath[m_particles.m_currentCount];

		--m_particles.m_currentCount;
	}

	void ParticleEmitterRuntime::SpawnParticle()
	{
		//spawn the particle randomly in a box transformed with sqt. So the center of the box is the position. Orientation
		//of the box is the rotation, the size of the box is the scale.

		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_real_distribution<float> dis(-0.5, 0.5);

		Core::Vec4f localPosition(dis(gen), dis(gen), dis(gen), 1);
		m_particles.m_timeToDeath[m_particles.m_currentCount] = m_lifetime;
		m_particles.m_position[m_particles.m_currentCount] = localPosition * m_transform.GetMatrix();
		m_particles.m_velocity[m_particles.m_currentCount] = Core::Vec4f(); //for now start with no velocity
		++m_particles.m_currentCount;
	}
}
