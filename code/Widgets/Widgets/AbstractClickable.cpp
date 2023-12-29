/********************************************************************/
/* © 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Widgets/Widgets/AbstractClickable.h"

#include "Widgets/Events/MouseEvent.h"

namespace Widgets
{
	AbstractClickable::AbstractClickable()
		: Widget()
		, m_mouseDownEvent(false)
	{}

	AbstractClickable::AbstractClickable(uint32_t w, uint32_t h, int32_t x, int32_t y)
		: Widget(w, h, x, y)
		, m_mouseDownEvent(false)
	{}

	AbstractClickable::~AbstractClickable()
	{}

	bool AbstractClickable::Handle(const BaseEvent& ev)
	{
		if (ev.m_id == EventType::kMouseDown)
		{
			const MouseEvent& mouseEvent = static_cast<const MouseEvent&>(ev);
			bool leftClick = mouseEvent.HasButton(MouseButton::LeftButton);
			if(leftClick)
				m_mouseDownEvent = true;

			return ParentClass::Handle(ev);
		}

		if (ev.m_id == EventType::kMouseExit)
		{
			m_mouseDownEvent = false;
			return ParentClass::Handle(ev);
		}

		if (ev.m_id == EventType::kMouseUp)
		{
			const MouseEvent& mouseEvent = static_cast<const MouseEvent&>(ev);
			bool leftClick = mouseEvent.HasButton(MouseButton::LeftButton);

			//first handle the mouse up event then do a click
			bool handled = ParentClass::Handle(ev);

			if (leftClick && m_mouseDownEvent && m_onClick)
			{
				m_onClick();
				handled = true;
			}

			return handled;
		}

		return ParentClass::Handle(ev);
	}
}
