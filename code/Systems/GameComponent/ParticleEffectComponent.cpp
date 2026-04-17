/********************************************************************************/
/* Copyright (C) 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Systems/GameComponent/ParticleEffectComponent.h"

#include "Systems/Game/World.h"
#include "Systems/Game/Subsystems/Clock/IClockSubsystem.h"
#include "Systems/Particle/ParticleSystem.h"
#include "Systems/Objects/GameObject.h"

namespace Systems
{
	ParticleEffectComponent::ParticleEffectComponent()
		: GameComponent()
		, m_effect()
		, m_effetHandle()
	{ }

	ParticleEffectComponent::~ParticleEffectComponent()
	{ }

	void ParticleEffectComponent::OnStart(World* pWorld)
	{
		TransformComponent& transform = GetOwner()->GetTransform();
		transform.ComputeWorldTx();

		const Core::Mat44f& worldTx = transform.GetWorldTx();
		m_effetHandle = pWorld->m_pParticleSystem->SpawnEffect(m_effect.GetPtr(), worldTx, pWorld->m_pClock->GetTime());
	}

	void ParticleEffectComponent::OnDestroy(World* pWorld)
	{
		pWorld->m_pParticleSystem->KillEffect(m_effetHandle);
	}
}