/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Core/Collections/Array.h"
#include "Core/Singleton.h"

#include "Systems/Assets/NewAssetId.h"
#include "Systems/Game/Subsystems/ISubsystem.h"
#include "Systems/Game/Subsystems/Particle/ParticleEffectHandle.h"

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
	class GameContext;
	class ParticleEffectAsset;
	class ParticleEmitterRuntime;
	class RenderableScene;

	class ParticleSystem : public ISubsystem
	{
	public:
		ParticleSystem();
		~ParticleSystem();

		ParticleEffectHandle SpawnEffect(ParticleEffectAsset* pEffect, const Core::Mat44f& world, float currentTime);

		void Play(ParticleEffectHandle handle, bool warmup = false);
		void Stop(ParticleEffectHandle handle);

		bool IsPlaying(ParticleEffectHandle handle) const;

		//Update the parameters of a currently running effect
		void UpdateEffectParameters(ParticleEffectHandle handle, ParticleEffectAsset* pEffect, const Core::Mat44f& world);

		void UpdateTransform(ParticleEffectHandle handle, const Core::Mat44f& world);

		void KillEffect(ParticleEffectHandle handle);
		void KillAllEffect();

		void Update(const GameContext& context) override;

		void BuildRenderable(RenderableScene& scene) override;

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

		ParticleEmitterRuntime* FindEffect(ParticleEffectHandle handle);
		const ParticleEmitterRuntime* FindEffect(ParticleEffectHandle handle) const;
	};
}
