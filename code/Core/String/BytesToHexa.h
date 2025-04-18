/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include <string>

namespace Core
{
	std::string Uint64ToHexa(uint64_t bytes);
	uint64_t HexaToUint64(const std::string hex);
}
