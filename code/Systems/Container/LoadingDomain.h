/********************************************************************************/
/* Copyright (C) 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include <cstdint>

namespace Systems
{
	enum class LoadingDomain : uint8_t
	{
		GAME = 0,
		EDITOR,

		COUNT,

		UNKNOWN = 255
	};
}
