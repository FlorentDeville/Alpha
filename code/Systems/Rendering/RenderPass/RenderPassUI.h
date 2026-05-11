/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Core/Math/Mat44f.h"
#include "Core/Math/Vectors.h"

#include "Systems/Rendering/RenderPass/IRenderPass.h"

#include <cstdint>

namespace Rendering
{
	class RenderTarget;
}

namespace Systems
{
	class RenderPassUI : public IRenderPass
	{
	public:
		RenderPassUI(uint32_t width, uint32_t height);
		~RenderPassUI();

		//void SetInput(Rendering::Texture* pInput);
		void SetOutput(Rendering::RenderTarget* pOutput);

		void PreRender(const RenderableScene& scene) override;
		void Render(const RenderableScene& scene) override;
		void PostRender(const RenderableScene& scene) override;

	private:
		Rendering::RenderTarget* m_pOutputRenderTarget;

		Core::Mat44f m_view;
		Core::Mat44f m_proj;
	};
}
