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
		float absValue = std::abs(value);

		int32_t integerPart = static_cast<int32_t>(std::trunc(absValue));
		int32_t decimalPart = static_cast<int32_t>(std::trunc((absValue - integerPart) * 1000));

		const char* pPositive = "";
		const char* pNegative = "-";
		const char* pSign = value < 0 ? pNegative : pPositive;

		if (decimalPart != 0)
			snprintf(pBuffer, bufferSize, "%s%d.%d", pSign, integerPart, decimalPart);
		else
			snprintf(pBuffer, bufferSize, "%s%d", pSign, integerPart);
	}
}
