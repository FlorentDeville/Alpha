/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Core/Reflection/ReflectionMacro.h"

#include "Systems/Assets/AssetRef/HardAssetRef.h"
#include "Systems/Objects/GameComponent.h"

ENABLE_REFLECTION(Systems, ParticleEffectComponent)

namespace Systems
{
	class ParticleEffectAsset;

	class ParticleEffectComponent : public GameComponent
	{
	public:
		ParticleEffectComponent();
		~ParticleEffectComponent();

		void OnStart(World* pWorld) override;
		void OnDestroy(World* pWorld) override;

	private:
		HardAssetRef<ParticleEffectAsset> m_effect;

		START_REFLECTION(Systems::ParticleEffectComponent)
			ADD_BASETYPE(Systems::GameComponent)
			ADD_FIELD(m_effect)
		END_REFLECTION()
	};
}