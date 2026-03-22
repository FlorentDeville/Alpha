/********************************************************************************/
/* Copyright (C) 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

namespace Widgets
{
	class Widget;
	class FocusEvent
	{
	public:
		FocusEvent(Widget* pFocusLost, Widget* pFocusGained)
			: m_pFocusLost(pFocusLost)
			, m_pFocusGained(pFocusGained)
		{ }

		Widget* m_pFocusLost;
		Widget* m_pFocusGained;
	};
}
