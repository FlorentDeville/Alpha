/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Widgets/Widgets/Container_V2.h"

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
	{
		m_virtualSize.x = INT32_MAX / 2;
		m_virtualSize.y = INT32_MAX / 2;
	}

	Container_V2::~Container_V2()
	{ }

	void Container_V2::Draw(const Core::Float2& windowSize, const D3D12_RECT& scissor)
	{
		D3D12_RECT innerRect;
		innerRect.left = m_absPos.x;
		innerRect.right = m_absPos.x + m_size.x;
		innerRect.top = m_absPos.y;
		innerRect.bottom = m_absPos.y + m_size.y;
		Parent::Draw(windowSize, innerRect);
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
			m_scrollingDistance.y = childrenMax.y - m_size.y;
	}
}
