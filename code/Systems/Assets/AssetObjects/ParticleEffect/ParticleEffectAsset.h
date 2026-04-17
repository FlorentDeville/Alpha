/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Core/Collections/Array.h"
#include "Core/Reflection/ReflectionMacro.h"

#include "Systems/Assets/AssetObjects/ParticleEffect/ParticleEmitter.h"
#include "Systems/Objects/AssetObject.h"

ENABLE_REFLECTION(Systems, ParticleEffectAsset)

namespace Systems
{
	class ParticleEmitter;

	class ParticleEffectAsset : public AssetObject
	{
	public:
		ParticleEffectAsset() = default;
		~ParticleEffectAsset() = default;

		const ParticleEmitter& GetEmitter() const;
		ParticleEmitter& GetEmitter();

		static const std::string& GetAssetTypeName();
		static Core::Sid GetAssetTypeNameSid();

	private:
		ParticleEmitter m_emitter;

		START_REFLECTION(Systems::ParticleEffectAsset)
			ADD_BASETYPE(Systems::AssetObject)
			ADD_FIELD(m_emitter)
		END_REFLECTION()
	};
}