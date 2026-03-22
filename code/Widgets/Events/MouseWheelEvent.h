/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include <cstdint>

namespace Widgets
{
	class MouseWheelEvent
	{
	public:
		MouseWheelEvent() = default;
		MouseWheelEvent(int32_t wheelDistance);
		~MouseWheelEvent() = default;

		int32_t GetWheelDistance() const;

	private:
		int32_t m_wheelDistance;
	};
}
