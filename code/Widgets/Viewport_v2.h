/********************************************************************/
/* © 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Widgets/Widget.h"

namespace Rendering
{
	class RenderTarget;
}

namespace Widgets
{
	class Viewport_v2 : public Widget
	{
	public:
		Viewport_v2(int width, int height);
		~Viewport_v2();

		void ClearDepthBuffer();

		void Update(uint64_t dt) override;

		void Draw(const DirectX::XMFLOAT2& windowSize, const D3D12_RECT& scissor) override;

		DirectX::XMVECTOR Compute3dPosition(const DirectX::XMUINT2& windowAbsPos) const;

		EVENT_DECL(Update, void(uint64_t)) //Real time update called every frame
		EVENT_DECL(Render, void()) //Do all your rendering inside this event

	protected:
		int m_width;
		int m_height;

		float m_aspectRatio;

	private:
		Rendering::RenderTarget* m_pRenderTarget; //the render target where this viewport will do the rendering

		bool m_updateEnabled; //true if the update event should be called

		void Internal_Render() override;
	};
}