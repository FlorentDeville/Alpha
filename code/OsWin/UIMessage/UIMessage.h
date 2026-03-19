/********************************************************************************/
/* Copyright (C) 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

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

		union High
		{
			uint64_t m_uint64;
			int64_t m_int64;
		};

		UIMessageId m_id;
		Low m_low;
		High m_high;
	};
}
