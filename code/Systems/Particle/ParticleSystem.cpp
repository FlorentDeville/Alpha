/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Systems/Particle/ParticleSystem.h"

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

	void ParticleSystem::SpawnEffect(const ParticleEffectAsset* pEffect)
	{
		//go through the emitters and spwn emitter runtime
	}

	void ParticleSystem::KillEffect(const ParticleEffectAsset* pEffect)
	{

	}

	void ParticleSystem::Update(float dtInSeconds)
	{
		for (TrackedEmitter& pEmitter : m_emitters)
			pEmitter.m_pEmitter->Update(dtInSeconds);
	}
}