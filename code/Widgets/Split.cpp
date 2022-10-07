/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Widgets/Split.h"
#include "Widgets/Message.h"

#include "Widgets/WidgetMgr.h"

#include "Rendering/Material/MaterialMgr.h"
#include "Rendering/RenderModule.h"

extern LPCWSTR g_pIconName;

namespace Widgets
{

Split::Split(bool isVerticalSplit)
	: Widget()
	, m_isDragged(false)
	, m_isVerticalSplit(isVerticalSplit)
{
	SetSizeStyle(Widget::HSIZE_DEFAULT | Widget::VSIZE_STRETCH);
	m_size.x = 10;
	m_size.y = 10;
}

Split::~Split()
{}

void Split::Draw(const DirectX::XMFLOAT2& windowSize)
{
	DirectX::XMMATRIX mvpMatrix;
	ComputeWVPMatrix(windowSize, mvpMatrix);

	DirectX::XMVECTOR color = DirectX::XMVectorSet(0.18f, 0.18f, 0.18f, 1);

	WidgetMgr& widgetMgr = WidgetMgr::Get();
	RenderModule& render = RenderModule::Get();
	Rendering::MaterialMgr& materialMgr = Rendering::MaterialMgr::Get();

	const Rendering::Material* pMaterial = materialMgr.GetMaterial(widgetMgr.m_materialId);
	render.BindMaterial(*pMaterial, mvpMatrix);

	render.SetConstantBuffer(1, sizeof(color), &color, 0);

	int value = 1;
	render.SetConstantBuffer(2, sizeof(value), &value, 0);
	render.SetConstantBuffer(3, sizeof(m_backgroundColor), &m_backgroundColor, 0);

	float rect[2] = { (float)m_size.x, (float)m_size.y };
	render.SetConstantBuffer(4, sizeof(rect), &rect, 0);

	m_borderWidth = 3;
	render.SetConstantBuffer(5, sizeof(m_borderWidth), &m_borderWidth, 0);

	const Mesh* pMesh = g_pMeshMgr->GetMesh(widgetMgr.m_quadMeshId);
	render.RenderMesh(*pMesh);

	Widget::Draw(windowSize);
}

bool Split::Handle(const Message& msg)
{
	switch (msg.m_id)
	{
	case M_MouseEnter:
	{
		if (m_isVerticalSplit)
			g_pIconName = IDC_SIZEWE;
		else
			g_pIconName = IDC_SIZENS;

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

		if (m_isVerticalSplit)
			g_pIconName = IDC_SIZEWE;
		else
			g_pIconName = IDC_SIZENS;
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
