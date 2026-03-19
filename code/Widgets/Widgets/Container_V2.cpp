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
		, m_showVScrollBar(false)
		, m_isDraggingScrollBar(false)
		, m_draggingStartMousePos(0, 0)
		, m_vScrollContainerSize(0)
		, m_hScrollContainerSize(0)
		, m_showHScrollBar(false)
	{

		const Color scrollContainerColor(46, 46, 46);
		const Color scrollBarColor(77, 77, 77);
		const Color scrollBarHoverColor(153, 153, 153);

		{
			m_pVScrollContainer = new Container();
			m_pVScrollContainer->SetSizeStyle(Widget::HSIZE_DEFAULT | Widget::VSIZE_DEFAULT);
			m_pVScrollContainer->SetSize(Core::UInt2(SCROLL_CONTAINER_SIZE, SCROLL_CONTAINER_SIZE));
			m_pVScrollContainer->SetPositionStyle(Widget::HPOSITION_STYLE::NONE, Widget::VPOSITION_STYLE::NONE);
			m_pVScrollContainer->GetDefaultStyle().SetBackgroundColor(scrollContainerColor);
			m_pVScrollContainer->GetHoverStyle().SetBackgroundColor(scrollContainerColor);
			m_pVScrollContainer->Disable();
			WidgetMgr::Get().RegisterWidget(m_pVScrollContainer);
		}

		{
			m_pVScrollBar = new Container();
			m_pVScrollBar->SetSizeStyle(Widget::HSIZE_DEFAULT | Widget::VSIZE_DEFAULT);
			m_pVScrollBar->SetSize(Core::UInt2(SCROLLBAR_CONTAINER_SIZE, SCROLLBAR_CONTAINER_SIZE));
			m_pVScrollBar->SetPositionStyle(Widget::HPOSITION_STYLE::NONE, Widget::VPOSITION_STYLE::NONE);
			m_pVScrollBar->GetDefaultStyle().SetBackgroundColor(scrollBarColor);
			m_pVScrollBar->GetHoverStyle().SetBackgroundColor(scrollBarHoverColor);
			m_pVScrollBar->Disable();
			m_pVScrollBar->OnMouseDown([this](const Widgets::MouseEvent& ev) { VScrollBar_OnMouseDown(ev); });
			m_pVScrollBar->OnMouseUp([this](const Widgets::MouseEvent& ev) { VScrollBar_OnMouseUp(ev); });
			m_pVScrollBar->OnMouseMove([this](const Widgets::MouseEvent& ev) { VScrollBar_OnMouseMove(ev); });
			WidgetMgr::Get().RegisterWidget(m_pVScrollBar);
		}

		{
			m_pHScrollContainer = new Container();
			m_pHScrollContainer->SetSizeStyle(Widget::HSIZE_DEFAULT | Widget::VSIZE_DEFAULT);
			m_pHScrollContainer->SetSize(Core::UInt2(SCROLL_CONTAINER_SIZE, SCROLL_CONTAINER_SIZE));
			m_pHScrollContainer->SetPositionStyle(Widget::HPOSITION_STYLE::NONE, Widget::VPOSITION_STYLE::NONE);
			m_pHScrollContainer->GetDefaultStyle().SetBackgroundColor(scrollContainerColor);
			m_pHScrollContainer->GetHoverStyle().SetBackgroundColor(scrollContainerColor);
			m_pHScrollContainer->Disable();
			WidgetMgr::Get().RegisterWidget(m_pHScrollContainer);
		}

		{
			m_pHScrollBar = new Container();
			m_pHScrollBar->SetSizeStyle(Widget::HSIZE_DEFAULT | Widget::VSIZE_DEFAULT);
			m_pHScrollBar->SetSize(Core::UInt2(SCROLLBAR_CONTAINER_SIZE, SCROLLBAR_CONTAINER_SIZE));
			m_pHScrollBar->SetPositionStyle(Widget::HPOSITION_STYLE::NONE, Widget::VPOSITION_STYLE::NONE);
			m_pHScrollBar->GetDefaultStyle().SetBackgroundColor(scrollBarColor);
			m_pHScrollBar->GetHoverStyle().SetBackgroundColor(scrollBarHoverColor);
			m_pHScrollBar->Disable();
			m_pHScrollBar->OnMouseDown([this](const Widgets::MouseEvent& ev) { HScrollBar_OnMouseDown(ev); });
			m_pHScrollBar->OnMouseUp([this](const Widgets::MouseEvent& ev) { HScrollBar_OnMouseUp(ev); });
			m_pHScrollBar->OnMouseMove([this](const Widgets::MouseEvent& ev) { HScrollBar_OnMouseMove(ev); });
			WidgetMgr::Get().RegisterWidget(m_pHScrollBar);
		}
	}

	Container_V2::~Container_V2()
	{
		WidgetMgr::Get().UnregisterWidget(m_pVScrollContainer);
		delete m_pVScrollContainer;
		m_pVScrollContainer = nullptr;

		WidgetMgr::Get().UnregisterWidget(m_pVScrollBar);
		delete m_pVScrollBar;
		m_pVScrollBar = nullptr;

		WidgetMgr::Get().UnregisterWidget(m_pHScrollContainer);
		delete m_pHScrollContainer;
		m_pHScrollContainer = nullptr;

		WidgetMgr::Get().UnregisterWidget(m_pHScrollBar);
		delete m_pHScrollBar;
		m_pHScrollBar = nullptr;
	}

	void Container_V2::Draw(const Core::Float2& windowSize, const D3D12_RECT& scissor)
	{
		D3D12_RECT fullRect;
		fullRect.left = m_absPos.x;
		fullRect.right = m_absPos.x + m_size.x;
		fullRect.top = m_absPos.y;
		fullRect.bottom = m_absPos.y + m_size.y;

		D3D12_RECT innerRect = fullRect;
		if(m_showVScrollBar)
			innerRect.right = m_absPos.x + m_size.x - SCROLL_CONTAINER_SIZE;
		if(m_showHScrollBar)
			innerRect.bottom = m_absPos.y + m_size.y - SCROLL_CONTAINER_SIZE;

		Parent::Draw(windowSize, innerRect);

		if (m_showVScrollBar)
		{
			m_pVScrollContainer->Draw(windowSize, fullRect);
			m_pVScrollBar->Draw(windowSize, fullRect);
		}

		if (m_showHScrollBar)
		{
			m_pHScrollContainer->Draw(windowSize, fullRect);
			m_pHScrollBar->Draw(windowSize, fullRect);
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

		if (m_showVScrollBar)
		{
			if(!m_showHScrollBar)
				m_vScrollContainerSize = m_size.y;
			else
				m_vScrollContainerSize = m_size.y - SCROLL_CONTAINER_SIZE;

			m_pVScrollContainer->Enable();
			m_pVScrollContainer->SetX(m_size.x - SCROLL_CONTAINER_SIZE);
			m_pVScrollContainer->SetY(0);
			m_pVScrollContainer->SetWidth(SCROLL_CONTAINER_SIZE);
			m_pVScrollContainer->SetHeight(m_vScrollContainerSize);
			m_pVScrollContainer->ReComputePosition(m_absPos, m_size);

			m_pVScrollBar->Enable();
			m_pVScrollBar->SetX(m_size.x - SCROLL_CONTAINER_SIZE + ((SCROLL_CONTAINER_SIZE - SCROLLBAR_CONTAINER_SIZE) / 2));
			UpdateVScrollBarPositionFromOffset();

			m_pVScrollBar->SetWidth(SCROLLBAR_CONTAINER_SIZE);
			m_pVScrollBar->SetHeight(m_vScrollContainerSize / 2);
			Core::Int3 absPos = m_absPos;
			absPos.z -= 1;
			m_pVScrollBar->ReComputePosition(absPos, m_size);
		}

		if (m_showHScrollBar)
		{
			if(!m_showVScrollBar)
				m_hScrollContainerSize = m_size.x;
			else
				m_hScrollContainerSize = m_size.x - SCROLL_CONTAINER_SIZE;

			m_pHScrollContainer->Enable();
			m_pHScrollContainer->SetX(0);
			m_pHScrollContainer->SetY(m_size.y - SCROLL_CONTAINER_SIZE);
			m_pHScrollContainer->SetWidth(m_hScrollContainerSize);
			m_pHScrollContainer->SetHeight(SCROLL_CONTAINER_SIZE);
			m_pHScrollContainer->ReComputePosition(m_absPos, m_size);

			m_pHScrollBar->Enable();
			{
				float ratio = -m_absPosOffset.x / static_cast<float>(m_scrollingDistance.x);
				int32_t scrollBarXPos = static_cast<int32_t>((m_hScrollContainerSize / 2) * ratio);
				m_pHScrollBar->SetX(scrollBarXPos);
			}

			m_pHScrollBar->SetY(m_size.y - SCROLL_CONTAINER_SIZE + ((SCROLL_CONTAINER_SIZE - SCROLLBAR_CONTAINER_SIZE) / 2));
			m_pHScrollBar->SetWidth(m_hScrollContainerSize / 2);
			m_pHScrollBar->SetHeight(SCROLLBAR_CONTAINER_SIZE);
			Core::Int3 absPos = m_absPos;
			absPos.z -= 1;
			m_pHScrollBar->ReComputePosition(absPos, m_size);
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

		Core::UInt2 innerSize = m_size;
		if (m_showVScrollBar)
			innerSize.x -= SCROLL_CONTAINER_SIZE;
		if (m_showHScrollBar)
			innerSize.y -= SCROLL_CONTAINER_SIZE;

		m_scrollingDistance = Core::UInt2(0, 0);
		if (innerSize.x < childrenMax.x)
		{
			m_scrollingDistance.x = childrenMax.x - innerSize.x;
			m_showHScrollBar = true;
		}
		else
		{
			m_showHScrollBar = false;
		}

		if (innerSize.y < childrenMax.y)
		{
			m_scrollingDistance.y = childrenMax.y - innerSize.y;
			m_showVScrollBar = true;
		}
		else
		{
			m_showVScrollBar = false;
		}

		m_absPosOffset.y = saturate(m_absPosOffset.y, -static_cast<int32_t>(m_scrollingDistance.y), 0);
		m_absPosOffset.x = saturate(m_absPosOffset.x, -static_cast<int32_t>(m_scrollingDistance.x), 0);
	}

	void Container_V2::UpdateVScrollBarPositionFromOffset()
	{
		float ratio = -m_absPosOffset.y / static_cast<float>(m_scrollingDistance.y);
		int32_t scrollBarYPos = static_cast<int32_t>((m_vScrollContainerSize / 2) * ratio);
		m_pVScrollBar->SetY(scrollBarYPos);
	}

	void Container_V2::VScrollBar_OnMouseDown(const Widgets::MouseEvent& ev)
	{
		m_pVScrollBar->CaptureMouse();
		m_isDraggingScrollBar = true;
		m_draggingStartMousePos = Core::Int2(ev.GetX(), ev.GetY());
		m_draggingStartPosOffset = m_absPosOffset;
	}

	void Container_V2::VScrollBar_OnMouseUp(const Widgets::MouseEvent& ev)
	{
		m_pVScrollBar->ReleaseMouse();
		m_isDraggingScrollBar = false;
	}

	void Container_V2::VScrollBar_OnMouseMove(const Widgets::MouseEvent& ev)
	{
		if (!m_isDraggingScrollBar)
			return;

		int32_t distance = ev.GetY() - m_draggingStartMousePos.y; //+ is down, - is up
		float ratio = distance / static_cast<float>((m_vScrollContainerSize / 2));
		m_absPosOffset.y = static_cast<int32_t>(-ratio * m_scrollingDistance.y) + m_draggingStartPosOffset.y;
		m_absPosOffset.y = saturate(m_absPosOffset.y, -static_cast<int32_t>(m_scrollingDistance.y), 0);

		WidgetMgr::Get().RequestResize();
	}

	void Container_V2::HScrollBar_OnMouseDown(const Widgets::MouseEvent& ev)
	{
		m_pHScrollBar->CaptureMouse();
		m_isDraggingScrollBar = true;
		m_draggingStartMousePos = Core::Int2(ev.GetX(), ev.GetY());
		m_draggingStartPosOffset = m_absPosOffset;
	}

	void Container_V2::HScrollBar_OnMouseUp(const Widgets::MouseEvent& ev)
	{
		m_pHScrollBar->ReleaseMouse();
		m_isDraggingScrollBar = false;
	}

	void Container_V2::HScrollBar_OnMouseMove(const Widgets::MouseEvent& ev)
	{
		if (!m_isDraggingScrollBar)
			return;

		int32_t distance = ev.GetX() - m_draggingStartMousePos.x; //+ is left, - is right
		float ratio = distance / static_cast<float>((m_hScrollContainerSize / 2));
		m_absPosOffset.x = static_cast<int32_t>(-ratio * m_scrollingDistance.x) + m_draggingStartPosOffset.x;
		m_absPosOffset.x = saturate(m_absPosOffset.x, -static_cast<int32_t>(m_scrollingDistance.x), 0);

		WidgetMgr::Get().RequestResize();
	}
}
