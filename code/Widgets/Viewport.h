/********************************************************************************/
/* Copyright (C) 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Widgets/Widget.h"

namespace Core
{
	class Vec4f;
}

namespace Rendering
{
	class RenderTarget;
}

namespace Widgets
{
	class Viewport : public Widget
	{
	public:
		Viewport(int width, int height);
		~Viewport();

		void ClearDepthBuffer();

		void Update(uint64_t dt) override;

		void Draw(const Core::Float2& windowSize, const D3D12_RECT& scissor) override;

		Core::Vec4f Compute3dPosition(const Core::UInt2& windowAbsPos) const;

		EVENT_DECL(Update, void(uint64_t)) //Real time update called every frame
		EVENT_DECL(Render, void()) //Do all your rendering inside this event

	protected:
		int m_width;
		int m_height;

		float m_aspectRatio;

		Rendering::RenderTarget* m_pRenderTarget; //the render target where this viewport will do the rendering

	private:

		bool m_updateEnabled; //true if the update event should be called

		void Internal_Render() override;
	};
}