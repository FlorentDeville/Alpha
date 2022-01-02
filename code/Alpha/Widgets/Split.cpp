/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Widgets/Split.h"
#include "Widgets/Message.h"

#include "Widgets/WidgetMgr.h"

#include "Rendering/Renderable/RenderableMgr.h"
#include "Rendering/RenderModule.h"

namespace Widgets
{

Split::Split()
	: Widget()
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

	DirectX::XMVECTOR color = DirectX::XMVectorSet(1, 0, 0, 1);
	//if (m_hover)
	//	color = DirectX::XMVectorSet(0.24f, 0.24f, 0.24f, 1.f);

	Renderable* pRenderable = g_pRenderableMgr->GetRenderable(g_pWidgetMgr->m_widgetRenderableId);
	g_pRenderModule->PreRenderForRenderable(*pRenderable);

	g_pRenderModule->SetConstantBuffer(0, sizeof(DirectX::XMMATRIX), &mvpMatrix, 0);
	g_pRenderModule->SetConstantBuffer(1, sizeof(color), &color, 0);

	//int value = m_showBorder ? 1 : 0;
	int value = 1;
	g_pRenderModule->SetConstantBuffer(2, sizeof(value), &value, 0);
	g_pRenderModule->SetConstantBuffer(3, sizeof(m_borderColor), &m_borderColor, 0);

	float rect[2] = { (float)m_size.x, (float)m_size.y };
	g_pRenderModule->SetConstantBuffer(4, sizeof(rect), &rect, 0);

	m_borderWidth = 3;
	g_pRenderModule->SetConstantBuffer(5, sizeof(m_borderWidth), &m_borderWidth, 0);

	g_pRenderModule->PostRenderForRenderable(*pRenderable);

	Widget::Draw();
}

//bool Split::Handle(const Message& msg)
//{
//	switch (msg.m_id)
//	{
//	case M_MouseLDown:
//	{
//		m_isDragged = true;
//
//		HCURSOR hCurs1 = LoadCursor(NULL, IDC_SIZEWE);
//		SetCursor(hCurs1);
//
//		return true;
//	}
//		break;
//
//	case M_MouseLUp:
//	{
//		m_isDragged = false;
//
//		HCURSOR hCurs1 = LoadCursor(NULL, IDC_ARROW);
//		SetCursor(hCurs1);
//
//		return true;
//	}
//		break;
//	}
//
//	return false;
//}
//
//bool Split::IsDragged() const
//{
//	return m_isDragged;
//}

}
