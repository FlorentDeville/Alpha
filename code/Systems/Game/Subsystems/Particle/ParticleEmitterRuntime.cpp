/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Systems/Game/Subsystems/Particle/ParticleEmitterRuntime.h"

#include "Rendering/PipelineState/PipelineStateDesc.h"
#include "Rendering/RenderModule.h"
#include "Rendering/RootSignature/RootSignatureMgr.h"
#include "Rendering/Shaders/ShaderMgr.h"
#include "Rendering/Texture/Texture.h"

#include "Systems/Assets/AssetObjects/Texture/Texture2DAsset.h"
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
		, m_pTexture(nullptr)
		, m_lastSpawnTime(0)
		, m_pIndexBuffer(nullptr)
		, m_pIndexBufferPtr(nullptr)
		, m_lsTx()
		, m_transform()
	{ }

	ParticleEmitterRuntime::~ParticleEmitterRuntime()
	{
		DeleteBuffers();
		delete m_pPso;
	}

	void ParticleEmitterRuntime::Init(uint32_t spawnRate, float lifetime, const Core::Vec4f& acceleration, const Core::Vec4f& speed, const Core::Mat44f& lsTx, 
		const Core::Mat44f& wsTx, float currentTime, Systems::Texture2DAsset* pTexture)
	{
		m_spawnRate = spawnRate;
		m_lifetime = lifetime;
		m_acceleration = acceleration;
		m_transform = wsTx;
		m_lsTx = lsTx;
		m_speed = speed;
		m_pTexture = pTexture;

		int iLifetime = static_cast<int>(std::ceil(lifetime));
		m_particles.m_maxCount = spawnRate * iLifetime;
		m_particles.m_currentCount = 0;

		CreateBuffers(m_particles.m_maxCount);

		m_pRootSig = Rendering::RootSignatureMgr::Get().GetRootSignature(Rendering::EngineRootSigs::PARTICLES);
		Rendering::Shader* pVs = Rendering::ShaderMgr::Get().GetShader(Rendering::EngineShaders::PARTICLES_VS);
		Rendering::Shader* pPs = Rendering::ShaderMgr::Get().GetShader(Rendering::EngineShaders::PARTICLES_PS);

		Rendering::PipelineStateDesc desc;
		desc.m_pPs = pPs;
		desc.m_pRs = m_pRootSig;
		desc.m_pVs = pVs;
		desc.m_blendDesc.m_blendEnabled = true;
		desc.m_blendDesc.m_blendOperation = Rendering::BlendOperation::ADD;
		desc.m_blendDesc.m_srcBlend = Rendering::BlendFactor::SRC_ALPHA;
		desc.m_blendDesc.m_dstBlend = Rendering::BlendFactor::INV_SRC_ALPHA;
		desc.m_blendDesc.m_blendOperationAlpha = Rendering::BlendOperation::ADD;
		desc.m_blendDesc.m_srcBlendAlpha = Rendering::BlendFactor::ONE;
		desc.m_blendDesc.m_dstBlendAlpha = Rendering::BlendFactor::ZERO;
		desc.m_depthStencilDesc.m_writeMask = Rendering::DepthWriteMask::ZERO;

		m_pPso = new Rendering::PipelineState();
		m_pPso->Init_Generic(desc);

		m_lastSpawnTime = -1;
	}

	void ParticleEmitterRuntime::UpdateParameters(uint32_t spawnRate, float lifetime, const Core::Vec4f& acceleration, const Core::Vec4f& speed, 
		const Core::Mat44f& transform, Systems::Texture2DAsset* pTexture)
	{
		m_spawnRate = spawnRate;
		m_lifetime = lifetime;
		m_acceleration = acceleration;
		m_transform = transform;
		m_speed = speed;
		m_pTexture = pTexture;

		int iLifetime = static_cast<int>(std::ceil(lifetime));
		int maxCount = spawnRate * iLifetime;
	
		if (maxCount != m_particles.m_maxCount)
		{
			m_particles.m_currentCount = 0;

			//new max count is bigger than the existing one so I need to resize all the buffers
			if (maxCount > m_particles.m_maxCount)
			{
				DeleteBuffers();
				CreateBuffers(maxCount);
			}

			m_particles.m_maxCount = maxCount;
		}
	}

	void ParticleEmitterRuntime::SetWsTransform(const Core::Mat44f& wsTx)
	{
		m_transform = m_lsTx * wsTx;
	}

	void ParticleEmitterRuntime::SetMaterial(Rendering::PipelineState* pPso, Rendering::RootSignature* pRootSig)
	{
		m_pPso = pPso;
		m_pRootSig = pRootSig;
	}

	void ParticleEmitterRuntime::Update(float currentTime, float dtInSeconds)
	{
		//first update, warmup
		if (m_lastSpawnTime == -1)
		{
			Warmup(currentTime, m_lifetime - 0.02f);
			m_lastSpawnTime = currentTime;
			return;
		}

		//kill dead particles particles
		for (int ii = 0; ii < m_particles.m_currentCount; ++ii)
		{
			if (m_particles.m_timeToDeath[ii] <= currentTime)
			{
				KillParticle(ii);

				// KillParticle will put the last particle in place of the deleted particle. So now index ii points to a particle not checked yet.
				--ii;
			}
		}

		//spawn new particles
		float accumulatedDt = (currentTime - m_lastSpawnTime);
		int particlesToSpawn = static_cast<int>(m_spawnRate * accumulatedDt);
		if (particlesToSpawn > 0)
			m_lastSpawnTime = currentTime;

		for (int ii = 0; ii < particlesToSpawn; ++ii)
		{
			if (m_particles.m_currentCount >= m_particles.m_maxCount)
				break;

			SpawnParticle(currentTime);
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
		//sort particle by camera distance
		for (int ii = 0; ii < m_particles.m_currentCount; ++ii)
			m_particles.m_indices[ii] = ii;

		//const Core::Vec4f& cameraPosition = scene.m_camera.m_position;
		//std::sort(m_particles.m_indices, m_particles.m_indices + m_particles.m_currentCount, [this, &cameraPosition](int a, int b)
		//	{
		//		//viewForward.dot(particlePos - cameraPos);
		//		float distanceA = (m_particles.m_position[a] - cameraPosition).Length2();
		//		float distanceB = (m_particles.m_position[b] - cameraPosition).Length2();

		//		return distanceA > distanceB;
		//	});

		memcpy(m_pIndexBufferPtr, m_particles.m_indices, sizeof(m_particles.m_indices[0]) * m_particles.m_currentCount);

		//createethe renderable
		Systems::RenderableParticles& renderable = scene.m_particles.PushBackDefault();
		renderable.m_pBufferPositions = m_pBufferPositions;
		renderable.m_pTexture = m_pTexture ? m_pTexture->GetTexture() : nullptr;
		renderable.m_pIndices = m_pIndexBuffer;
		renderable.m_instanceCount = m_particles.m_currentCount;
		renderable.m_pPso = m_pPso;
		renderable.m_pRootsig = m_pRootSig;
	}

	void ParticleEmitterRuntime::Warmup(float currentTime, float warmupDuration)
	{
		int particlesToSpawn = static_cast<int>(m_spawnRate * warmupDuration);

		float singleParticleDt = warmupDuration / particlesToSpawn;

		float startTime = currentTime - warmupDuration;
		for (int ii = 0; ii < particlesToSpawn; ++ii)
		{
			if (m_particles.m_currentCount >= m_particles.m_maxCount)
				break;

			SpawnParticle(startTime + (singleParticleDt * ii));
		}

		//update position
		// I use p = p0 + v0t + 1/2 a t*t. I can't use the same code as in the update because it takes the assumption
		// the speed is constant over dt. That's ok in the update, over a small dt. But here it gives incorrect result.
		for (int ii = 0; ii < m_particles.m_currentCount; ++ii)
		{
			float dt = warmupDuration - static_cast<float>(singleParticleDt * ii);
			m_particles.m_position[ii] = m_particles.m_position[ii] + m_particles.m_velocity[ii] * dt + m_acceleration * 0.5f * dt * dt;
		}

		//update velocity
		for (int ii = 0; ii < m_particles.m_currentCount; ++ii)
			m_particles.m_velocity[ii] = m_particles.m_velocity[ii] + (m_acceleration * (warmupDuration - static_cast<float>(singleParticleDt * ii)));
	}

	void ParticleEmitterRuntime::KillParticle(int index)
	{
		--m_particles.m_currentCount;

		m_particles.m_position[index] = m_particles.m_position[m_particles.m_currentCount];
		m_particles.m_velocity[index] = m_particles.m_velocity[m_particles.m_currentCount];
		m_particles.m_timeToDeath[index] = m_particles.m_timeToDeath[m_particles.m_currentCount];
	}

	void ParticleEmitterRuntime::SpawnParticle(float currentTime)
	{
		//spawn the particle randomly in a box transformed with sqt. So the center of the box is the position. Orientation
		//of the box is the rotation, the size of the box is the scale.

		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_real_distribution<float> dis(-0.5, 0.5);

		Core::Vec4f localPosition(dis(gen), dis(gen), dis(gen), 1);
		m_particles.m_timeToDeath[m_particles.m_currentCount] = currentTime + m_lifetime;
		m_particles.m_position[m_particles.m_currentCount] = localPosition * m_transform;
		m_particles.m_velocity[m_particles.m_currentCount] = m_speed;
		++m_particles.m_currentCount;
	}

	void ParticleEmitterRuntime::CreateBuffers(int maxCount)
	{
		m_particles.m_position = new Core::Vec4f[maxCount];
		m_particles.m_velocity = new Core::Vec4f[maxCount];
		m_particles.m_timeToDeath = new float[maxCount];
		m_particles.m_indices = new uint16_t[maxCount];

		m_pBufferPositions = new Rendering::Texture();
		m_pBufferPositions->InitAsParticlesBuffer(sizeof(Core::Vec4f), maxCount);
		m_pGfxBufferPositions = m_pBufferPositions->Map();

		m_pIndexBuffer = new Rendering::Texture();
		m_pIndexBuffer->InitAsParticlesBuffer(sizeof(uint16_t), maxCount);
		m_pIndexBufferPtr = m_pIndexBuffer->Map();
	}

	void ParticleEmitterRuntime::DeleteBuffers()
	{
		delete[] m_particles.m_position;
		delete[] m_particles.m_timeToDeath;
		delete[] m_particles.m_velocity;
		delete[] m_particles.m_indices;

		delete m_pBufferPositions;
		delete m_pIndexBuffer;
	}
}
