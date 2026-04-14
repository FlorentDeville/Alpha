/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Systems/Particle/ParticleSystem.h"

#include "Systems/Assets/AssetObjects/ParticleEffect/ParticleEffectAsset.h"
#include "Systems/Assets/AssetObjects/ParticleEffect/ParticleEmitter.h"
#include "Systems/Particle/ParticleEmitterRuntime.h"

namespace Systems
{
	ParticleSystem::ParticleSystem()
		: m_emitters()
	{ }

	ParticleSystem::~ParticleSystem()
	{
		for (TrackedEmitter& pEmitter : m_emitters)
			delete pEmitter.m_pEmitter;
	}

	void ParticleSystem::SpawnEffect(const ParticleEffectAsset* pEffect, const Core::Mat44f& world)
	{
		//go through the emitters and spawn emitter runtime
		const Core::Array<ParticleEmitter*>& emitterAssetList = pEffect->GetEmitters();
		for (const ParticleEmitter* pEmitterAsset : emitterAssetList)
		{
			TrackedEmitter& runtimeEmitter = m_emitters.PushBackDefault();
			runtimeEmitter.m_id = pEffect->GetId();
			runtimeEmitter.m_pEmitter = new ParticleEmitterRuntime();

			const Core::Float3& emitterAcceleration = pEmitterAsset->GetAcceleration();
			Core::Vec4f acceleration(emitterAcceleration.x, emitterAcceleration.y, emitterAcceleration.z, 0);
			
			Core::Mat44f finalTransform = pEmitterAsset->GetTransform().GetMatrix() * world;
			runtimeEmitter.m_pEmitter->Init(pEmitterAsset->GetSpawnRate(), pEmitterAsset->GetLifetime(), acceleration, finalTransform);
		}
	}

	void ParticleSystem::KillEffect(const ParticleEffectAsset* pEffect)
	{
		for (uint32_t ii = 0; ii < m_emitters.GetSize(); ++ii)
		{
			if (m_emitters[ii].m_id != pEffect->GetId())
				continue;

			m_emitters[ii] = m_emitters.Back();
			m_emitters.Resize(m_emitters.GetSize() - 1);

			--ii;
		}
	}

	void ParticleSystem::Update(float dtInSeconds)
	{
		for (TrackedEmitter& pEmitter : m_emitters)
			pEmitter.m_pEmitter->Update(dtInSeconds);
	}
}