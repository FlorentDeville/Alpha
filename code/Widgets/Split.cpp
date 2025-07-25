/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Widgets/Split.h"

#include "OsWin/Cursor/Cursor.h"

#include "Rendering/Material/MaterialMgr.h"
#include "Rendering/Mesh/MeshMgr.h"
#include "Rendering/RenderModule.h"

#include "Widgets/Events/BaseEvent.h"
#include "Widgets/Events/MouseEvent.h"
#include "Widgets/WidgetMgr.h"

extern OsWin::CursorId g_pIconName;

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

void Split::Draw(const Core::Float2& windowSize, const D3D12_RECT& scissor)
{
	DirectX::XMMATRIX mvpMatrix;
	ComputeWVPMatrix(windowSize, mvpMatrix);

	DirectX::XMVECTOR color = DirectX::XMVectorSet(0.18f, 0.18f, 0.18f, 1);

	WidgetMgr& widgetMgr = WidgetMgr::Get();
	Rendering::RenderModule& render = Rendering::RenderModule::Get();
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

	const Rendering::Mesh* pMesh = Rendering::MeshMgr::Get().GetMesh(widgetMgr.m_quadMeshId);
	render.RenderMesh(*pMesh);

	Widget::Draw(windowSize, scissor);
}

bool Split::Handle(const BaseEvent& ev)
{
	switch (ev.m_id)
	{
	case EventType::kMouseEnter:
	{
		if (m_isVerticalSplit)
			g_pIconName = OsWin::CursorId::ResizeHorizontal;
		else
			g_pIconName = OsWin::CursorId::ResizeVertical;

		return true;
	}
		break;

	case EventType::kMouseExit:
	{
		if (!m_isDragged)
			g_pIconName = OsWin::CursorId::Arrow;

		return true;
	}
		break;

	case EventType::kMouseUp:
	{
		//left button
		const MouseEvent& mouseEvent = static_cast<const MouseEvent&>(ev);
		if (mouseEvent.HasButton(MouseButton::LeftButton))
		{
			ReleaseMouse();

			if (!IsInside(mouseEvent.GetX(), mouseEvent.GetY()))
				g_pIconName = OsWin::CursorId::Arrow;

			m_isDragged = false;
			return true;
		}
	}
	break;

	case EventType::kMouseDown:
	{
		const MouseEvent& mouseEvent = static_cast<const MouseEvent&>(ev);
		if (mouseEvent.HasButton(MouseButton::LeftButton) && !m_isDragged)
		{
			CaptureMouse();

			m_isDragged = true;
			m_previousCursorPosition.x = mouseEvent.GetX();
			m_previousCursorPosition.y = mouseEvent.GetY();

			if (m_isVerticalSplit)
				g_pIconName = OsWin::CursorId::ResizeHorizontal;
			else
				g_pIconName = OsWin::CursorId::ResizeVertical;
			return true;
		}
	}
	break;

	case EventType::kMouseMove:
	{
		if (m_isDragged)
		{
			const MouseEvent& mouseEvent = static_cast<const MouseEvent&>(ev);

			Core::Int2 currentMousePos(mouseEvent.GetX(), mouseEvent.GetY());
			m_onDrag(currentMousePos);

			m_previousCursorPosition.x = currentMousePos.x;
			m_previousCursorPosition.y = currentMousePos.y;
		}
	}
	break;

	}

	return Widget::Handle(ev);
}

bool Split::IsDragged()
{
	return m_isDragged;
}

Core::Int2 Split::GetPreviousCursorPosition() const
{
	return m_previousCursorPosition;
}

void Split::SetPreviousCursorPosition(const Core::Int2& pos)
{
	m_previousCursorPosition = pos;
}

}
