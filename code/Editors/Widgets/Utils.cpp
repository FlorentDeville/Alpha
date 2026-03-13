/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Editors/Widgets/Utils.h"

#include <cmath>
#include <cstdio>

namespace Editors
{
	void Utils::FToA(float value, char* pBuffer, uint32_t bufferSize)
	{
		float roundedValue = static_cast<int32_t>(std::round(value * 1000)) * 0.001f;

		snprintf(pBuffer, bufferSize, "%g", roundedValue);
	}
}
