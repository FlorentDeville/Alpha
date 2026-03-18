/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Widgets/Widgets/Container_V2.h"

#include "Widgets/Container.h"
#include "Widgets/Events/MouseWheelEvent.h"
#include "Widgets/WidgetMgr.h"

namespace Widgets
{
	static int32_t saturate(int32_t value, int32_t min, int32_t max)
	{
		if (value < min) return min;
		if (value > max) return max;
		return value;
	}

	Container_V2::Container_V2()
		: Widget()
		, m_absPosOffset(0, 0)
		, m_scrollingDistance(0, 0)
		, m_showVerticalScrollBar(false)
		, m_isDraggingVerticalScrollBar(false)
		, m_dragVScrollBarStartPos(0, 0)
	{
		m_virtualSize.x = INT32_MAX / 2;
		m_virtualSize.y = INT32_MAX / 2;

		{
			m_pVerticalScrollContainer = new Container();
			m_pVerticalScrollContainer->SetSizeStyle(Widget::HSIZE_DEFAULT | Widget::VSIZE_DEFAULT);
			m_pVerticalScrollContainer->SetSize(Core::UInt2(SCROLL_CONTAINER_SIZE, SCROLL_CONTAINER_SIZE));
			m_pVerticalScrollContainer->SetPositionStyle(Widget::HPOSITION_STYLE::NONE, Widget::VPOSITION_STYLE::NONE);

			const Color scrollContainerColor(46, 46, 46);
			m_pVerticalScrollContainer->GetDefaultStyle().SetBackgroundColor(scrollContainerColor);
			m_pVerticalScrollContainer->GetHoverStyle().SetBackgroundColor(scrollContainerColor);
			m_pVerticalScrollContainer->Disable();
			WidgetMgr::Get().RegisterWidget(m_pVerticalScrollContainer);
		}

		{
			m_pVerticalScrollBar = new Container();
			m_pVerticalScrollBar->SetSizeStyle(Widget::HSIZE_DEFAULT | Widget::VSIZE_DEFAULT);
			m_pVerticalScrollBar->SetSize(Core::UInt2(SCROLLBAR_CONTAINER_SIZE, SCROLLBAR_CONTAINER_SIZE));
			m_pVerticalScrollBar->SetPositionStyle(Widget::HPOSITION_STYLE::NONE, Widget::VPOSITION_STYLE::NONE);

			m_pVerticalScrollBar->GetDefaultStyle().SetBackgroundColor(Color(77, 77, 77));
			m_pVerticalScrollBar->GetHoverStyle().SetBackgroundColor(Color(153, 153, 153));
			m_pVerticalScrollBar->Disable();
			m_pVerticalScrollBar->OnMouseDown([this](const Widgets::MouseEvent& ev) { VScrollBar_OnMouseDown(ev); });
			m_pVerticalScrollBar->OnMouseUp([this](const Widgets::MouseEvent& ev) { VScrollBar_OnMouseUp(ev); });
			m_pVerticalScrollBar->OnMouseMove([this](const Widgets::MouseEvent& ev) { VScrollBar_OnMouseMove(ev); });
			WidgetMgr::Get().RegisterWidget(m_pVerticalScrollBar);
		}
	}

	Container_V2::~Container_V2()
	{
		WidgetMgr::Get().UnregisterWidget(m_pVerticalScrollContainer);
		delete m_pVerticalScrollContainer;
		m_pVerticalScrollContainer = nullptr;

		WidgetMgr::Get().UnregisterWidget(m_pVerticalScrollBar);
		delete m_pVerticalScrollBar;
		m_pVerticalScrollBar = nullptr;
	}

	void Container_V2::Draw(const Core::Float2& windowSize, const D3D12_RECT& scissor)
	{
		D3D12_RECT fullRect;
		fullRect.left = m_absPos.x;
		fullRect.right = m_absPos.x + m_size.x;
		fullRect.top = m_absPos.y;
		fullRect.bottom = m_absPos.y + m_size.y;

		D3D12_RECT innerRect = fullRect;
		if(m_showVerticalScrollBar)
			innerRect.right = m_absPos.x + m_size.x - SCROLL_CONTAINER_SIZE;
	
		Parent::Draw(windowSize, innerRect);

		if (m_showVerticalScrollBar)
		{
			m_pVerticalScrollContainer->Draw(windowSize, fullRect);
			m_pVerticalScrollBar->Draw(windowSize, fullRect);
		}
	}

	bool Container_V2::Handle(const BaseEvent& ev)
	{
		switch (ev.m_id)
		{
		case EventType::kMouseWheel:
		{
			if (m_scrollingDistance.y == 0)
				return true;

			const int INCREMENT = 20;
			const MouseWheelEvent& wheelEvent = static_cast<const MouseWheelEvent&>(ev);
			if (wheelEvent.GetWheelDistance() > 0) //forward
			{
				m_absPosOffset.y += INCREMENT;
			}
			else //backward
			{
				m_absPosOffset.y -= INCREMENT;
			}

			m_absPosOffset.y = saturate(m_absPosOffset.y, -static_cast<int32_t>(m_scrollingDistance.y), 0);

			Widgets::WidgetMgr::Get().RequestResize();
			return true;
		}
		break;

		default:
			return Parent::Handle(ev);
		break;

		}
	}

	void Container_V2::Resize(const Core::Int3& parentAbsPos, const Core::UInt2& parentSize)
	{
		Parent::Resize(parentAbsPos, parentSize);

		if (m_showVerticalScrollBar)
		{
			m_pVerticalScrollContainer->Enable();
			m_pVerticalScrollContainer->SetX(m_size.x - SCROLL_CONTAINER_SIZE);
			m_pVerticalScrollContainer->SetY(0);
			m_pVerticalScrollContainer->SetWidth(SCROLL_CONTAINER_SIZE);
			m_pVerticalScrollContainer->SetHeight(m_size.y);
			m_pVerticalScrollContainer->ReComputePosition(m_absPos, m_size);

			m_pVerticalScrollBar->Enable();
			m_pVerticalScrollBar->SetX(m_size.x - SCROLL_CONTAINER_SIZE + ((SCROLL_CONTAINER_SIZE - SCROLLBAR_CONTAINER_SIZE) / 2));
			UpdateVScrollBarPositionFromOffset();

			m_pVerticalScrollBar->SetWidth(SCROLLBAR_CONTAINER_SIZE);
			m_pVerticalScrollBar->SetHeight(m_size.y / 2);
			Core::Int3 absPos = m_absPos;
			absPos.z -= 1;
			m_pVerticalScrollBar->ReComputePosition(absPos, m_size);
		}
	}

	void Container_V2::ResizeChildren()
	{
		Core::UInt2 childrenMax = Core::UInt2(0, 0);

		for (Widget* pChild : m_children)
		{
			Core::Int3 absPos = m_absPos;
			absPos.x += m_absPosOffset.x;
			absPos.y += m_absPosOffset.y;

			pChild->Resize(absPos, m_virtualSize);
			childrenMax.x = max(childrenMax.x, pChild->GetX() + pChild->GetWidth());
			childrenMax.y = max(childrenMax.y, pChild->GetY() + pChild->GetHeight());
		}

		m_scrollingDistance = Core::UInt2(0, 0);
		if (m_size.x < childrenMax.x)
			m_scrollingDistance.x = childrenMax.x - m_size.x;

		if (m_size.y < childrenMax.y)
		{
			m_scrollingDistance.y = childrenMax.y - m_size.y;
			m_showVerticalScrollBar = true;
		}
		else
		{
			m_showVerticalScrollBar = false;
		}
	}

	void Container_V2::UpdateVScrollBarPositionFromOffset()
	{
		float ratio = -m_absPosOffset.y / static_cast<float>(m_scrollingDistance.y);
		int32_t scrollBarYPos = static_cast<int32_t>((m_size.y / 2) * ratio);
		m_pVerticalScrollBar->SetY(scrollBarYPos);
	}

	void Container_V2::VScrollBar_OnMouseDown(const Widgets::MouseEvent& ev)
	{
		m_pVerticalScrollBar->CaptureMouse();
		m_isDraggingVerticalScrollBar = true;
		m_dragVScrollBarStartPos = Core::Int2(ev.GetX(), ev.GetY());
	}

	void Container_V2::VScrollBar_OnMouseUp(const Widgets::MouseEvent& ev)
	{
		m_pVerticalScrollBar->ReleaseMouse();
		m_isDraggingVerticalScrollBar = false;
	}

	void Container_V2::VScrollBar_OnMouseMove(const Widgets::MouseEvent& ev)
	{
		if (!m_isDraggingVerticalScrollBar)
			return;

		int32_t distance = ev.GetY() - m_dragVScrollBarStartPos.y; //+ is down, - is up

		int32_t newPos = m_pVerticalScrollBar->GetY() + distance;
		newPos = saturate(newPos, 0, m_size.y / 2);
		float ratio = newPos / static_cast<float>((m_size.y / 2));
		m_absPosOffset.y = static_cast<int32_t>(-ratio * m_scrollingDistance.y);
		m_dragVScrollBarStartPos = Core::Int2(ev.GetX(), ev.GetY());
		m_pVerticalScrollBar->SetY(newPos);

		WidgetMgr::Get().RequestResize();
	}
}
