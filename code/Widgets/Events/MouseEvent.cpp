/********************************************************************/
/* © 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Widgets/Events/MouseEvent.h"

namespace Widgets
{
	MouseEvent::MouseEvent(EventType id, int x, int y, MouseButton button)
		: m_x(x)
		, m_y(y)
		, m_button(button)
	{
		m_id = id;
	}

	MouseEvent::~MouseEvent()
	{}

	int MouseEvent::GetX() const
	{
		return m_x;
	}

	int MouseEvent::GetY() const
	{
		return m_y;
	}

	MouseButton MouseEvent::GetButton() const
	{
		return m_button;
	}

	bool MouseEvent::HasButton(MouseButton button)const
	{
		return (m_button & button) != 0;
	}
}
