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

	void Container::Draw()
	{
		DirectX::XMMATRIX mvpMatrix;
		ComputeWVPMatrix(mvpMatrix);

		DirectX::XMVECTOR color = m_backgroundColor;

		Renderable* pRenderable = g_pRenderableMgr->GetRenderable(WidgetMgr::Get().m_widgetRenderableId);
		g_pRenderModule->PreRenderForRenderable(*pRenderable);

		g_pRenderModule->SetConstantBuffer(0, sizeof(DirectX::XMMATRIX), &mvpMatrix, 0);
		g_pRenderModule->SetConstantBuffer(1, sizeof(color), &color, 0);

		int value = 0;
		g_pRenderModule->SetConstantBuffer(2, sizeof(value), &value, 0);
		g_pRenderModule->SetConstantBuffer(3, sizeof(m_borderColor), &m_borderColor, 0);

		float rect[2] = { (float)m_size.x, (float)m_size.y };
		g_pRenderModule->SetConstantBuffer(4, sizeof(rect), &rect, 0);

		m_borderWidth = 3;
		g_pRenderModule->SetConstantBuffer(5, sizeof(m_borderWidth), &m_borderWidth, 0);

		g_pRenderModule->PostRenderForRenderable(*pRenderable);

		Widget::Draw();
	}
}
