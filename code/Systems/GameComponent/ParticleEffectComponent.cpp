/********************************************************************************/
/* Copyright (C) 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Systems/GameComponent/ParticleEffectComponent.h"

#include "Systems/Game/World.h"
#include "Systems/Particle/ParticleSystem.h"
#include "Systems/Objects/GameObject.h"

namespace Systems
{
	ParticleEffectComponent::ParticleEffectComponent()
		: GameComponent()
		, m_effect()
	{ }

	ParticleEffectComponent::~ParticleEffectComponent()
	{ }

	void ParticleEffectComponent::OnStart(World* pWorld)
	{
		//const Core::Mat44f& worldTx = GetOwner()->GetTransform().GetWorldTx();
		//pWorld->m_pParticleSystem->SpawnEffect(m_effect.GetPtr(), worldTx, )
	}

	void ParticleEffectComponent::OnDestroy(World* pWorld)
	{

	}
}