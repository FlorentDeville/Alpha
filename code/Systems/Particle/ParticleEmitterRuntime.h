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

			int m_maxCount;
			int m_currentCount;
		};

		ParticleEmitterRuntime();
		~ParticleEmitterRuntime();

		void Init(uint32_t spawnRate, float lifetime, const Core::Vec4f& acceleration, const Core::Vec4f& speed, const Core::Mat44f& transform, float currentTime, 
			Systems::Texture2DAsset* pTexture);

		void UpdateParameters(uint32_t spawnRate, float lifetime, const Core::Vec4f& acceleration, const Core::Vec4f& speed, const Core::Mat44f& transform, Systems::Texture2DAsset* pTexture);

		void SetMaterial(Rendering::PipelineState* pPso, Rendering::RootSignature* pRootSig);

		void Update(float currentTime, float dtInSeconds);

		void Upload();

		void BuildRenderable(RenderableScene& scene);

	private:
		Particles m_particles;
		Core::Mat44f m_transform;
		Core::Vec4f m_acceleration;
		Core::Vec4f m_speed;
		int m_spawnRate;
		float m_lifetime;

		//rendering
		Rendering::Texture* m_pBufferPositions; //I use a texture as a buffer. Not great but it handles the srv automatically. Good enough for now.
		void* m_pGfxBufferPositions;

		//hardcode material for now
		Rendering::PipelineState* m_pPso;
		Rendering::RootSignature* m_pRootSig;
		Systems::Texture2DAsset* m_pTexture;
		float m_lastSpawnTime;

		void KillParticle(int index);
		void SpawnParticle(float currentTime);
	};
}
