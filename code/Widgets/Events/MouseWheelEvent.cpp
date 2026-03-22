/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Widgets/Events/MouseWheelEvent.h"

namespace Widgets
{
	MouseWheelEvent::MouseWheelEvent(int32_t wheelDistance)
		: m_wheelDistance(wheelDistance)
	{ }

	int32_t MouseWheelEvent::GetWheelDistance() const
	{
		return m_wheelDistance;
	}
}
