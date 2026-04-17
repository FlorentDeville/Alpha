/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Core/Collections/Array.h"
#include "Core/Singleton.h"

#include "Systems/Assets/NewAssetId.h"
#include "Systems/Particle/ParticleEffectHandle.h"

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

		ParticleEffectHandle SpawnEffect(ParticleEffectAsset* pEffect, const Core::Mat44f& world, float currentTime);
		void KillEffect(ParticleEffectHandle handle);

		void KillAllEffect();

		void Update(float currentTime);

		void BuildRenderable(RenderableScene& scene);

	private:
		class TrackedEmitter
		{
		public:
			ParticleEmitterRuntime* m_pEmitter;
			uint32_t m_generation;
			bool m_free;
		};

		Core::Array<TrackedEmitter> m_emitters;

		//hardcode material for now
		Rendering::PipelineState* m_pPso;
		Rendering::RootSignature* m_pRootSig;

		float m_lastUpdate;

		uint32_t FindFreeSlot();
	};
}
