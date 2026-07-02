/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include <cstdint>

namespace Systems
{
	class RenderSettings
	{
	public:
		static uint32_t s_msaaSampleCount;
		static uint32_t s_msaaQualityLevel;
	};
}
