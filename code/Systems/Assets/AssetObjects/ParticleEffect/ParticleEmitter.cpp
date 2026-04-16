/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Systems/Assets/AssetObjects/ParticleEffect/ParticleEmitter.h"

#include "Systems/Assets/AssetObjects/Texture/Texture2DAsset.h"

namespace Systems
{
	const Core::Sqt& ParticleEmitter::GetTransform() const
	{
		return m_transform;
	}

	const Core::Float3& ParticleEmitter::GetSpeed() const
	{
		return m_speed;
	}

	const Core::Float3& ParticleEmitter::GetAcceleration() const
	{
		return m_acceleration;
	}

	float ParticleEmitter::GetLifetime() const
	{
		return m_lifetime;
	}

	uint32_t ParticleEmitter::GetSpawnRate() const
	{
		return m_spawnRate;
	}

	Systems::Texture2DAsset* ParticleEmitter::GetTexture()
	{
		return m_texture.GetPtr();
	}
}
