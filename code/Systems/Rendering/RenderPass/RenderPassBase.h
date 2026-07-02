/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Systems/Rendering/RenderPass/IRenderPass.h"

#include <cstdint>

namespace Rendering
{
	class DescriptorHeap;
	class RenderTarget;
}

namespace Systems
{
	class RenderPassBase : public IRenderPass
	{
	public:
		RenderPassBase(uint32_t width, uint32_t height);
		~RenderPassBase();

		void SetShadowMapRenderTargets(Rendering::RenderTarget** pShadowRenderTargets, uint32_t count, Rendering::DescriptorHeap* pShadowMapSrvHeap);

		void PreRender(const RenderableScene& scene);
		void Render(const RenderableScene& scene);
		void PostRender(const RenderableScene& scene);

		void ClearDepthBuffer();

		Rendering::RenderTarget* GetRenderTarget();

	private:
		Rendering::RenderTarget* m_pRenderTarget;

		Rendering::DescriptorHeap* m_pShadowMapSrvHeap;
		Rendering::RenderTarget** m_pShadowRenderTargets;
		uint32_t m_shadowRenderTargetsCount;

		void RenderMsaaScene(const RenderableScene& scene);
	};
}
