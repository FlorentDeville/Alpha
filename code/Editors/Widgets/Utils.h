/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include <cstdint>

namespace Editors
{
	class Utils
	{
	public:
		static void FToA(float value, char* pBuffer, uint32_t bufferSize);
	};
}
