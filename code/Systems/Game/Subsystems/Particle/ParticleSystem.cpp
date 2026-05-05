/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Systems/Game/Subsystems/Particle/ParticleSystem.h"

#include "Systems/Assets/AssetObjects/ParticleEffect/ParticleEffectAsset.h"
#include "Systems/Assets/AssetObjects/ParticleEffect/ParticleEmitter.h"
#include "Systems/Game/GameContext.h"
#include "Systems/Game/Subsystems/Clock/IClockSubsystem.h"
#include "Systems/Game/Subsystems/Particle/ParticleEmitterRuntime.h"

namespace Systems
{
	ParticleSystem::ParticleSystem()
		: m_emitters()
		, m_pPso(nullptr)
		, m_pRootSig(nullptr)
		, m_lastUpdate(0)
	{ }

	ParticleSystem::~ParticleSystem()
	{
		for (TrackedEmitter& emitter : m_emitters)
		{
			if (emitter.m_free)
				continue;

			delete emitter.m_pEmitter;
		}
	}

	ParticleEffectHandle ParticleSystem::SpawnEffect(ParticleEffectAsset* pEffect, const Core::Mat44f& world, float currentTime)
	{
		//go through the emitters and spawn emitter runtime
		ParticleEmitter& emitter = pEffect->GetEmitter();

		if (emitter.GetSpawnRate() == 0 || emitter.GetLifetime() == 0)
			return ParticleEffectHandle();

		uint32_t index = FindFreeSlot();
		TrackedEmitter& trackedEmitter = m_emitters[index];
		trackedEmitter.m_free = false;
		++trackedEmitter.m_generation;
		trackedEmitter.m_pEmitter = new ParticleEmitterRuntime();

		const Core::Float3& emitterAcceleration = emitter.GetAcceleration();
		Core::Vec4f acceleration(emitterAcceleration.x, emitterAcceleration.y, emitterAcceleration.z, 0);
			
		const Core::Float3& emitterSpeed = emitter.GetSpeed();
		Core::Vec4f speed(emitterSpeed.x, emitterSpeed.y, emitterSpeed.z, 0);

		Core::Mat44f finalTransform = emitter.GetTransform().GetMatrix() * world;

		trackedEmitter.m_pEmitter->Init(emitter.GetSpawnRate(), emitter.GetLifetime(), acceleration, speed, finalTransform, currentTime, emitter.GetTexture());

		ParticleEffectHandle handle;
		handle.m_index = index;
		handle.m_generation = trackedEmitter.m_generation;
		return handle;
	}

	void ParticleSystem::UpdateEffectParameters(ParticleEffectHandle handle, ParticleEffectAsset* pEffect, const Core::Mat44f& world)
	{
		//find the emitter
		if (!m_emitters.IsValidIndex(handle.m_index))
			return;

		TrackedEmitter& tracker = m_emitters[handle.m_index];
		if (tracker.m_generation != tracker.m_generation)
			return;

		ParticleEmitterRuntime* pEmitter = tracker.m_pEmitter;

		//now update the values
		ParticleEmitter& emitterAsset = pEffect->GetEmitter();
		const Core::Float3& emitterAcceleration = emitterAsset.GetAcceleration();
		Core::Vec4f acceleration(emitterAcceleration.x, emitterAcceleration.y, emitterAcceleration.z, 0);

		const Core::Float3& emitterSpeed = emitterAsset.GetSpeed();
		Core::Vec4f speed(emitterSpeed.x, emitterSpeed.y, emitterSpeed.z, 0);

		Core::Mat44f finalTransform = emitterAsset.GetTransform().GetMatrix() * world;

		pEmitter->UpdateParameters(emitterAsset.GetSpawnRate(), emitterAsset.GetLifetime(), acceleration, speed, finalTransform, emitterAsset.GetTexture());
	}

	void ParticleSystem::KillEffect(ParticleEffectHandle handle)
	{
		if (!m_emitters.IsValidIndex(handle.m_index))
			return;

		TrackedEmitter& emitter = m_emitters[handle.m_index];
		if (emitter.m_generation != handle.m_generation)
			return;

		delete emitter.m_pEmitter;
		emitter.m_free = true;
	}

	void ParticleSystem::KillAllEffect()
	{
		for(TrackedEmitter& emitter : m_emitters)
		{
			if (emitter.m_free)
				continue;

			delete emitter.m_pEmitter;
			emitter.m_free = true;
		}
	}

	void ParticleSystem::Update(const GameContext& context)
	{
		float currentTime = context.m_pClock->GetTime();

		float dt = 0;
		if (m_lastUpdate != 0)
			dt = currentTime - m_lastUpdate;

		for (TrackedEmitter& pEmitter : m_emitters)
		{
			if (pEmitter.m_free)
				continue;

			pEmitter.m_pEmitter->Update(currentTime, dt);
			pEmitter.m_pEmitter->Upload();
		}

		m_lastUpdate = currentTime;
	}

	void ParticleSystem::BuildRenderable(RenderableScene& scene)
	{
		for (TrackedEmitter& pEmitter : m_emitters)
		{
			if (pEmitter.m_free)
				continue;

			pEmitter.m_pEmitter->BuildRenderable(scene);
		}
	}

	uint32_t ParticleSystem::FindFreeSlot()
	{
		for(uint32_t ii = 0; ii < m_emitters.GetSize(); ++ii)
		{
			if (m_emitters[ii].m_free)
				return ii;
		}

		uint32_t id = m_emitters.GetSize();
		TrackedEmitter& emitter = m_emitters.PushBackDefault();
		emitter.m_free = true;
		emitter.m_generation = 0;
		emitter.m_pEmitter = nullptr;
		return id;
	}
}