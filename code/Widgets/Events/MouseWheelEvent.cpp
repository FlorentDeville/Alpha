/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Widgets/Events/MouseWheelEvent.h"

namespace Widgets
{
	MouseWheelEvent::MouseWheelEvent(int32_t wheelDistance)
		: BaseEvent()
		, m_wheelDistance(wheelDistance)
	{
		m_id = EventType::kMouseWheel;
	}

	int32_t MouseWheelEvent::GetWheelDistance()
	{
		return m_wheelDistance;
	}
}
