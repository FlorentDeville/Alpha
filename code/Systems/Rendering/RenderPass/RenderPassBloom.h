/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Core/Math/Vectors.h"

#include "Systems/Rendering/RenderPass/IRenderPass.h"

#include <cstdint>

namespace Rendering
{
	class PipelineState;
	class RenderTarget;
	class RootSignature;
	class Texture;
}

namespace Systems
{
	class RenderPassBloom : public IRenderPass
	{
	public:
		RenderPassBloom(uint32_t width, uint32_t height, uint32_t mipCount);
		~RenderPassBloom();

		void SetInput(Rendering::Texture* pInput);
		void SetOutput(Rendering::RenderTarget* pOutput);

		void PreRender(const RenderableScene& scene) override;
		void Render(const RenderableScene& scene) override;
		void PostRender(const RenderableScene& scene) override;

	private:
		float m_bloomFilterRadius; //radius for the upsampling

		uint32_t m_mipCount;
		Core::Float2* m_mipSizes;

		Rendering::Texture* m_pInputTexture;
		Rendering::RenderTarget* m_pOutputRenderTarget;

		Rendering::RenderTarget* m_pEmissive;
		Rendering::PipelineState* m_pEmissiveFilterPso;
		Rendering::RootSignature* m_pEmissiveFilterRootSig;

		Rendering::RenderTarget** m_ppMipRenderTarget;

		Rendering::PipelineState* m_pDownsamplingPso;
		Rendering::RootSignature* m_pDownsamplingRootSig;

		Rendering::PipelineState* m_pUpsamplingPso;
		Rendering::RootSignature* m_pUpsamplingRootSig;
	};
}
