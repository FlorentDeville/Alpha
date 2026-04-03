/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Systems/Rendering/RenderPass/IRenderPass.h"

#include <cstdint>

namespace Rendering
{
	class RenderTarget;
	class Texture;
}

namespace Systems
{
	class RenderPassBloom : public IRenderPass
	{
	public:
		RenderPassBloom(uint32_t width, uint32_t height, uint32_t mipCount);
		~RenderPassBloom();

		void SetFrameBuffer(Rendering::Texture* pFrameBuffer);

		void PreRender(const RenderableScene& scene) override;
		void Render(const RenderableScene& scene) override;
		void PostRender(const RenderableScene& scene) override;

	private:
		uint32_t m_mipCount;

		Rendering::RenderTarget** m_ppRenderTarget;
		Rendering::Texture* m_pFrameBuffer;
	};
}
