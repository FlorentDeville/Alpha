/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Core/Math/Sqt.h"
#include "Core/Math/Vectors.h"
#include "Core/Reflection/ReflectionMacro.h"

#include "Systems/Assets/AssetRef/HardAssetRef.h"

ENABLE_REFLECTION(Systems, ParticleEmitter)

namespace Systems
{
	class MaterialInstanceAsset;

	class ParticleEmitter
	{
	public:
		ParticleEmitter() = default;
		~ParticleEmitter() = default;

	private:
		Core::Sqt m_transform;

		HardAssetRef<Systems::MaterialInstanceAsset> m_materialInstance;

		Core::Float3 m_speed;
		Core::Float3 m_acceleration;

		float m_lifetime;
		float m_spawnRate;

		START_REFLECTION(Systems::ParticleEmitter)
			ADD_FIELD(m_transform)
			ADD_FIELD(m_materialInstance)
			ADD_FIELD(m_speed)
			ADD_FIELD(m_acceleration)
			ADD_FIELD(m_lifetime)
			ADD_FIELD(m_spawnRate)
		END_REFLECTION()
	};
}
