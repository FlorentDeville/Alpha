/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include <cstdint>

namespace Rendering
{
	class BufferHandle
	{
	public:
		uint32_t m_index;
		uint8_t m_generation;
	};
}
