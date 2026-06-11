/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Core/Math/Mat44f.h"
#include "Core/Math/Vec4f.h"

namespace Core
{
	class Vec4f;
}

namespace Rendering
{
	class PipelineState;
	class RootSignature;
	class Texture;
}

namespace Systems
{
	class RenderableScene;
	class Texture2DAsset;

	class ParticleEmitterRuntime
	{
	public:

		class Particles
		{
		public:
			Particles() = default;

			Core::Vec4f* m_position;
			Core::Vec4f* m_velocity;
			float* m_timeToDeath;
			uint16_t* m_indices;

			int m_maxCount;
			int m_currentCount;
		};

		ParticleEmitterRuntime();
		~ParticleEmitterRuntime();

		void Init(uint32_t spawnRate, float lifetime, const Core::Vec4f& acceleration, const Core::Vec4f& speed, const Core::Mat44f& lsTx,
			const Core::Mat44f& wsTx, float currentTime, Systems::Texture2DAsset* pTexture);

		void UpdateParameters(uint32_t spawnRate, float lifetime, const Core::Vec4f& acceleration, const Core::Vec4f& speed, const Core::Mat44f& transform, Systems::Texture2DAsset* pTexture);

		void SetWsTransform(const Core::Mat44f& wsTx);
		void SetMaterial(Rendering::PipelineState* pPso, Rendering::RootSignature* pRootSig);

		void Update(float currentTime, float dtInSeconds);

		void Upload();

		void BuildRenderable(RenderableScene& scene);

		void Warmup(float currentTime, float warmupDuration);

		void Play(bool warmup = false);
		void Stop();

		bool IsPlaying() const;

	private:
		Particles m_particles;
		Core::Mat44f m_lsTx;			// local space transform coming from the effect asset
		Core::Mat44f m_transform;		// world space transform, m_lsTx * parentSpace
		Core::Vec4f m_acceleration;
		Core::Vec4f m_speed;
		int m_spawnRate;
		float m_lifetime;

		bool m_emittingStateRequested;
		bool m_isEmitting;
		bool m_warmupRequested;
		
		//rendering
		Rendering::Texture* m_pBufferPositions; //I use a texture as a buffer. Not great but it handles the srv automatically. Good enough for now.
		Rendering::Texture* m_pIndexBuffer; //Sorted index buffer by camera distance
		void* m_pGfxBufferPositions;
		void* m_pIndexBufferPtr;

		//hardcode material for now
		Rendering::PipelineState* m_pPso;
		Rendering::RootSignature* m_pRootSig;
		Systems::Texture2DAsset* m_pTexture;
		float m_lastSpawnTime;

		void KillParticle(int index);
		void SpawnParticle(float currentTime);

		void CreateBuffers(int maxCount);
		void DeleteBuffers();
	};
}
