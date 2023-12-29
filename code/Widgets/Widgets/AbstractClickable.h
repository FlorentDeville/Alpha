/********************************************************************/
/* © 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Widgets/Widget.h"

namespace Widgets
{
	class AbstractClickable : public Widget
	{
	public:
		using ParentClass = Widget;

		AbstractClickable();
		AbstractClickable(uint32_t w, uint32_t h, int32_t x, int32_t y);
		~AbstractClickable();

		bool Handle(const BaseEvent& event) override;

		EVENT_DECL(Click, void())

	private:
		bool m_mouseDownEvent;
	};
}
