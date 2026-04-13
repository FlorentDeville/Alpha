/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Core/Collections/Array.h"
#include "Core/Reflection/ReflectionMacro.h"

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

		static const std::string& GetAssetTypeName();
		static Core::Sid GetAssetTypeNameSid();

	private:
		Core::Array<ParticleEmitter*> m_emitters;

		START_REFLECTION(Systems::ParticleEffectAsset)
			ADD_BASETYPE(Systems::AssetObject)
			ADD_FIELD(m_emitters)
		END_REFLECTION()
	};
}