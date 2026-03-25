/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Systems/Rendering/RenderPass/IRenderPass.h"

#include <string>

namespace Rendering
{
	class DescriptorHeap;
	class RenderTarget;
}

namespace Systems
{
	class ShadowMapsImpl;

	class RenderPassShadowMaps : public IRenderPass
	{
	public:
		RenderPassShadowMaps();
		~RenderPassShadowMaps();

		void PreRender(const RenderableScene& scene);
		void Render(const RenderableScene& scene);
		void PostRender(const RenderableScene& scene);

		uint32_t GetRenderTargetsCount() const;
		Rendering::RenderTarget** GetRenderTargets();

		Rendering::DescriptorHeap* GetSrvHeap();

	private:
		void CreateRenderTargets();

		ShadowMapsImpl* m_pImpl;
	};
}
