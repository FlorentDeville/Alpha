/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Widgets/Split.h"
#include "Widgets/Message.h"

#include "Widgets/WidgetMgr.h"

#include "Rendering/Renderable/RenderableMgr.h"
#include "Rendering/RenderModule.h"

extern LPCWSTR g_pIconName;

namespace Widgets
{

Split::Split()
	: Widget()
	, m_isDragged(false)
{
	SetSizeStyle(Widget::HSIZE_DEFAULT | Widget::VSIZE_STRETCH);
	m_size.x = 10;
}

Split::~Split()
{}

void Split::Draw()
{
	DirectX::XMMATRIX mvpMatrix;
	ComputeWVPMatrix(mvpMatrix);

	DirectX::XMVECTOR color = DirectX::XMVectorSet(0.18f, 0.18f, 0.18f, 1);

	Renderable* pRenderable = g_pRenderableMgr->GetRenderable(WidgetMgr::Get().m_widgetRenderableId);

	RenderModule& render = RenderModule::Get();

	render.PreRenderForRenderable(*pRenderable);

	render.SetConstantBuffer(0, sizeof(DirectX::XMMATRIX), &mvpMatrix, 0);
	render.SetConstantBuffer(1, sizeof(color), &color, 0);

	int value = 1;
	render.SetConstantBuffer(2, sizeof(value), &value, 0);
	render.SetConstantBuffer(3, sizeof(m_backgroundColor), &m_backgroundColor, 0);

	float rect[2] = { (float)m_size.x, (float)m_size.y };
	render.SetConstantBuffer(4, sizeof(rect), &rect, 0);

	m_borderWidth = 3;
	render.SetConstantBuffer(5, sizeof(m_borderWidth), &m_borderWidth, 0);

	render.PostRenderForRenderable(*pRenderable);

	Widget::Draw();
}

bool Split::Handle(const Message& msg)
{
	switch (msg.m_id)
	{
	case M_MouseEnter:
	{
		g_pIconName = IDC_SIZEWE;
		return true;
	}
		break;

	case M_MouseExit:
	{
		if (!m_isDragged)
			g_pIconName = IDC_ARROW;

		return true;
	}
		break;

	case M_MouseLUp:
	{
		ReleaseMouse();

		if(!IsInside(msg.m_low.m_pos[0], msg.m_low.m_pos[1]))
			g_pIconName = IDC_ARROW;

		m_isDragged = false;
		return true;
	}
	break;

	case M_MouseLDown:
	{
		CaptureMouse();

		m_isDragged = true;
		m_previousCursorPosition = WidgetMgr::Get().GetCursorPosition();

		g_pIconName = IDC_SIZEWE;
		return true;
	}
	break;

	}

	return Widget::Handle(msg);
}

bool Split::IsDragged()
{
	return m_isDragged;
}

DirectX::XMINT2 Split::GetPreviousCursorPosition() const
{
	return m_previousCursorPosition;
}

void Split::SetPreviousCursorPosition(const DirectX::XMINT2& pos)
{
	m_previousCursorPosition = pos;
}

}
