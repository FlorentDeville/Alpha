/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Systems/Particle/ParticleEmitterRuntime.h"

#include "Rendering/PipelineState/PipelineStateDesc.h"
#include "Rendering/RenderModule.h"
#include "Rendering/RootSignature/RootSignatureMgr.h"
#include "Rendering/Shaders/ShaderMgr.h"
#include "Rendering/Texture/Texture.h"

#include "Systems/Rendering/Renderable/RenderableParticles.h"
#include "Systems/Rendering/Renderable/RenderableScene.h"

#include <cmath>
#include <random>

namespace Systems
{
	ParticleEmitterRuntime::ParticleEmitterRuntime()
		: m_spawnRate()
		, m_lifetime()
		, m_acceleration()
		, m_particles()
		, m_pGfxBufferPositions(nullptr)
		, m_pBufferPositions(nullptr)
		, m_pPso(nullptr)
		, m_pRootSig(nullptr)
	{ }

	ParticleEmitterRuntime::~ParticleEmitterRuntime()
	{
		delete[] m_particles.m_position;
		delete[] m_particles.m_timeToDeath;
		delete[] m_particles.m_velocity;

		delete m_pBufferPositions;
		delete m_pPso;
	}

	void ParticleEmitterRuntime::Init(uint32_t spawnRate, float lifetime, const Core::Vec4f& acceleration, const Core::Vec4f& speed, const Core::Mat44f& transform, float currentTime)
	{
		m_spawnRate = spawnRate;
		m_lifetime = lifetime;
		m_acceleration = acceleration;
		m_transform = transform;
		m_speed = speed;

		int iLifetime = static_cast<int>(std::ceil(lifetime));
		m_particles.m_maxCount = spawnRate * iLifetime;

		m_particles.m_position = new Core::Vec4f[m_particles.m_maxCount];
		m_particles.m_velocity = new Core::Vec4f[m_particles.m_maxCount];
		m_particles.m_timeToDeath = new float[m_particles.m_maxCount];

		m_pBufferPositions = new Rendering::Texture();
		m_pBufferPositions->InitAsParticlesBuffer(sizeof(Core::Vec4f), m_particles.m_maxCount);
		m_pGfxBufferPositions = m_pBufferPositions->Map();

		m_pRootSig = Rendering::RootSignatureMgr::Get().GetRootSignature(Rendering::EngineRootSigs::PARTICLES);
		Rendering::Shader* pVs = Rendering::ShaderMgr::Get().GetShader(Rendering::EngineShaders::PARTICLES_VS);
		Rendering::Shader* pPs = Rendering::ShaderMgr::Get().GetShader(Rendering::EngineShaders::PARTICLES_PS);

		Rendering::PipelineStateDesc desc;
		desc.m_pPs = pPs;
		desc.m_pRs = m_pRootSig;
		desc.m_pVs = pVs;

		m_pPso = new Rendering::PipelineState();
		m_pPso->Init_Generic(desc);

		m_lastSpawnTime = currentTime;
	}

	void ParticleEmitterRuntime::SetMaterial(Rendering::PipelineState* pPso, Rendering::RootSignature* pRootSig)
	{
		m_pPso = pPso;
		m_pRootSig = pRootSig;
	}

	void ParticleEmitterRuntime::Update(float currentTime, float dtInSeconds)
	{
		//kill dead particles particles
		for (int ii = 0; ii < m_particles.m_currentCount; ++ii)
		{
			m_particles.m_timeToDeath[ii] -= dtInSeconds;
			if (m_particles.m_timeToDeath[ii] <= 0)
			{
				KillParticle(ii);

				// KillParticle will put the last particle in place of the deleted particle. So now index ii points to a particle not checked yet.
				--ii;
			}
		}

		//spawn new particles
		float accumulatedDt = (currentTime - m_lastSpawnTime) + dtInSeconds;

		int particlesToSpawn = static_cast<int>(m_spawnRate * accumulatedDt);
		if (particlesToSpawn > 0)
			m_lastSpawnTime = currentTime;

		for (int ii = 0; ii < particlesToSpawn; ++ii)
		{
			if (m_particles.m_currentCount >= m_particles.m_maxCount)
				break;

			SpawnParticle();
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
		memcpy(m_pGfxBufferPositions, m_particles.m_position, sizeof(m_particles.m_position[0]) * m_particles.m_currentCount);
	}

	void ParticleEmitterRuntime::BuildRenderable(RenderableScene& scene)
	{
		Systems::RenderableParticles& renderable = scene.m_particles.PushBackDefault();
		renderable.m_pBufferPositions = m_pBufferPositions;
		renderable.m_instanceCount = m_particles.m_currentCount;
		renderable.m_pPso = m_pPso;
		renderable.m_pRootsig = m_pRootSig;
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
		m_particles.m_position[m_particles.m_currentCount] = localPosition * m_transform;
		m_particles.m_velocity[m_particles.m_currentCount] = m_speed; //for now start with no velocity
		++m_particles.m_currentCount;
	}
}
