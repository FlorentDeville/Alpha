/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Widgets/Events/EventType.h"
#include "Widgets/Events/KeyboardEvent.h"
#include "Widgets/Events/MouseEvent.h"
#include "Widgets/Events/MouseWheelEvent.h"

namespace Widgets
{
	class EventStorage
	{
	public:
		union Param
		{
			KeyboardEvent m_keyboardEvent;
			MouseEvent m_mouseEvent;
			MouseWheelEvent m_mouseWheelEvent;
		};

		EventType m_id;
		Param m_param;
	};
}
