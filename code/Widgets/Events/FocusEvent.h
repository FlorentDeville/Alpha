/********************************************************************/
/* © 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Widgets/Events/BaseEvent.h"

namespace Widgets
{
	class Widget;
	class FocusEvent : public BaseEvent
	{
	public:
		FocusEvent(Widget* pFocusLost, Widget* pFocusGained)
			: BaseEvent()
			, m_pFocusLost(pFocusLost)
			, m_pFocusGained(pFocusGained)
		{
			m_id = EventType::kFocusChanged;
		}

		Widget* m_pFocusLost;
		Widget* m_pFocusGained;
	};
}
