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

		void Draw(const DirectX::XMFLOAT2& windowSize) override;

		DirectX::XMVECTOR Compute3dPosition(const DirectX::XMUINT2& windowAbsPos) const;

		EVENT_DECL(Render, void()) //Do all your rendering inside this event

	protected:
		int m_width;
		int m_height;

		float m_aspectRatio;

	private:
		Rendering::RenderTarget* m_pRenderTarget; //the render target where this viewport will do the rendering

		void Internal_Render() override;
	};
}