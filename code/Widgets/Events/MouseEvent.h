/********************************************************************/
/* � 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Widgets/Events/BaseEvent.h"

namespace Widgets
{
	enum MouseButton
	{
		NoButton = 0,
		LeftButton = 0x1,
		RightButton = 0x2,
		MiddleButton = 0x4
	};

	class MouseEvent : public BaseEvent
	{
	public:
		MouseEvent(EventType id, int x, int y, MouseButton button);
		~MouseEvent();
		
		//Mouse position in ui coordinate (origin is the top left of the screen)
		int GetX() const;

		//Mouse position in ui coordinate (origin is the top left of the screen)
		int GetY() const; 

		//Indicate what button triggered this event
		MouseButton GetButton() const;

		//Check if button triggered this event
		bool HasButton(MouseButton button)const;

	private:
		int m_x;
		int m_y;

		MouseButton m_button;
	};
}
