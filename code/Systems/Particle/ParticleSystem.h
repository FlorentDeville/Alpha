/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Core/Collections/Array.h"
#include "Core/Singleton.h"

#include "Systems/Assets/NewAssetId.h"

namespace Core
{
	class Mat44f;
}

namespace Systems
{
	class ParticleEffectAsset;
	class ParticleEmitterRuntime;

	class ParticleSystem
	{
	public:
		ParticleSystem();
		~ParticleSystem();

		void SpawnEffect(const ParticleEffectAsset* pEffect, const Core::Mat44f& world);
		void KillEffect(const ParticleEffectAsset* pEffect);

		void Update(float dtInSeconds);

	private:
		class TrackedEmitter
		{
		public:
			NewAssetId m_id;
			ParticleEmitterRuntime* m_pEmitter;
		};

		Core::Array<TrackedEmitter> m_emitters;
	};
}
