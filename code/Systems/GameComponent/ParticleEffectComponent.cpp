/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Systems/GameComponent/ParticleEffectComponent.h"

#include "Systems/Game/GameContext.h"
#include "Systems/Game/Subsystems/Clock/IClockSubsystem.h"
#include "Systems/Game/Subsystems/Particle/ParticleSystem.h"
#include "Systems/Objects/GameObject.h"

namespace Systems
{
	ParticleEffectComponent::ParticleEffectComponent()
		: GameComponent()
		, m_effect()
		, m_effetHandle()
		, m_warmup(false)
	{ }

	ParticleEffectComponent::~ParticleEffectComponent()
	{ }

	void ParticleEffectComponent::OnStart(GameContext* pWorld)
	{
		TransformComponent& transform = GetOwner()->GetTransform();
		transform.ComputeWorldTx();

		m_effetHandle = ParticleEffectHandle();

		SpawnEffect(pWorld);
	}

	void ParticleEffectComponent::OnDestroy(GameContext* pWorld)
	{
		pWorld->m_pParticleSystem->KillEffect(m_effetHandle);
	}

	void ParticleEffectComponent::SpawnEffect(GameContext* pWorld)
	{
		//already spawned
		if (m_effetHandle.IsValid())
			return;

		TransformComponent& transform = GetOwner()->GetTransform();

		const Core::Mat44f& worldTx = transform.GetWorldTx();
		m_effetHandle = pWorld->m_pParticleSystem->SpawnEffect(m_effect.GetPtr(), worldTx, pWorld->m_pClock->GetTime());
		pWorld->m_pParticleSystem->Play(m_effetHandle, m_warmup);
	}

	ParticleEffectAsset* ParticleEffectComponent::GetEffectAsset()
	{
		return m_effect.GetPtr();
	}

	ParticleEffectHandle ParticleEffectComponent::GetEffectRutimeHandle() const
	{
		return m_effetHandle;
	}
}