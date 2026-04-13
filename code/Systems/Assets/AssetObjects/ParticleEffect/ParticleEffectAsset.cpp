/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Systems/Assets/AssetObjects/ParticleEffect/ParticleEffectAsset.h"

#include "Systems/Assets/AssetObjects/ParticleEffect/ParticleEmitter.h"

namespace Systems
{
	ParticleEffectAsset::~ParticleEffectAsset()
	{
		for (ParticleEmitter* pEmitter : m_emitters)
			delete pEmitter;
	}

	const std::string& ParticleEffectAsset::GetAssetTypeName()
	{
		static std::string name = "ParticleEffect";
		return name;
	}

	Core::Sid ParticleEffectAsset::GetAssetTypeNameSid()
	{
		static Core::Sid sid = SID(GetAssetTypeName());
		return sid;
	}
}