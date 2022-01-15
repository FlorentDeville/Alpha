/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Widgets/MessageId.h"

#include <cstdint>

struct Message
{
	union Low
	{
		uint32_t m_pos[2];
		uint64_t m_raw;
	};

	MessageId m_id;
	Low m_low;
	uint64_t m_high;
};
