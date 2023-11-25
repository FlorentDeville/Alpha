/********************************************************************/
/* © 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Widgets/Events/BaseEvent.h"

namespace Widgets
{
	class MouseEvent : public BaseEvent
	{
	public:
		int m_x;
		int m_y;
	};
}
