/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Widgets/Container.h"

#include "Rendering/RenderModule.h"
#include "Rendering/Renderable/RenderableMgr.h"

#include "Widgets/WidgetMgr.h"

namespace Widgets
{
	Container::Container()
		: Widget()
	{
		SetSizeStyle(Widget::HSIZE_STRETCH | Widget::VSIZE_STRETCH);
	}

	Container::Container(int width, int height)
		: Widget(width, height, 0, 0)
	{}

	Container::~Container()
	{}

	void Container::Draw(const DirectX::XMFLOAT2& windowSize)
	{
		DirectX::XMMATRIX mvpMatrix;
		ComputeWVPMatrix(windowSize, mvpMatrix);

		DirectX::XMVECTOR color = m_backgroundColor;

		Renderable* pRenderable = g_pRenderableMgr->GetRenderable(WidgetMgr::Get().m_widgetRenderableId);

		RenderModule& render = RenderModule::Get();
		render.PreRenderForRenderable(*pRenderable);

		render.SetConstantBuffer(0, sizeof(DirectX::XMMATRIX), &mvpMatrix, 0);
		render.SetConstantBuffer(1, sizeof(color), &color, 0);

		int value = 0;
		render.SetConstantBuffer(2, sizeof(value), &value, 0);
		render.SetConstantBuffer(3, sizeof(m_borderColor), &m_borderColor, 0);

		float rect[2] = { (float)m_size.x, (float)m_size.y };
		render.SetConstantBuffer(4, sizeof(rect), &rect, 0);

		m_borderWidth = 3;
		render.SetConstantBuffer(5, sizeof(m_borderWidth), &m_borderWidth, 0);

		render.PostRenderForRenderable(*pRenderable);

		Widget::Draw(windowSize);
	}
}
