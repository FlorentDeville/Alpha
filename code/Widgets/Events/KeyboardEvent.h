/********************************************************************/
/* © 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Widgets/Events/BaseEvent.h"

namespace Widgets
{
	class KeyboardEvent : public BaseEvent
	{
	public:
		char m_virtualKey;
	};
}
