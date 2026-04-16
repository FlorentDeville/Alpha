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

namespace Rendering
{
	class PipelineState;
	class RootSignature;
}

namespace Systems
{
	class ParticleEffectAsset;
	class ParticleEmitterRuntime;
	class RenderableScene;

	class ParticleSystem
	{
	public:
		ParticleSystem();
		~ParticleSystem();

		void SpawnEffect(const ParticleEffectAsset* pEffect, const Core::Mat44f& world, float currentTime);
		void KillEffect(const ParticleEffectAsset* pEffect);

		void KillAllEffect();

		void Update(float currentTime, float dtInSeconds);

		void BuildRenderable(RenderableScene& scene);

	private:
		class TrackedEmitter
		{
		public:
			NewAssetId m_id;
			ParticleEmitterRuntime* m_pEmitter;
		};

		Core::Array<TrackedEmitter> m_emitters;

		//hardcode material for now
		Rendering::PipelineState* m_pPso;
		Rendering::RootSignature* m_pRootSig;
	};
}
