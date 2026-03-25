/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

namespace Systems
{
	class RenderableScene;

	class IRenderPass
	{
	public:
		IRenderPass() = default;
		virtual ~IRenderPass() = default;

		virtual void PreRender(const RenderableScene& scene) = 0;
		virtual void Render(const RenderableScene& scene) = 0;
		virtual void PostRender(const RenderableScene& scene) = 0;
	};
}
