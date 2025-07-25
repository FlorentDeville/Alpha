/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "OsWin/UIMessage/UIMessageId.h"

#include <cstdint>

namespace OsWin
{
	// Wrapper around windows ui messages so the rest of the application doesn't have to deal with it.
	class UIMessage
	{
	public:
		union Low
		{
			uint32_t m_uint32[2];
			uint64_t m_uint64;
		};

		UIMessageId m_id;
		Low m_low;
		uint64_t m_high;
	};
}
