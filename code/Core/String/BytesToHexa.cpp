/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Core/String/BytesToHexa.h"

#include <iomanip>
#include <sstream>

namespace Core
{
	// Convert a uint64_t into a hexadecimal string filled with leading 0 so the size of the string
	// is constant (16 characters)
	std::string Uint64ToHexa(uint64_t bytes)
	{
		//This is bad for performance but at least it's wrapped up in a function.
		//Optimize it when needed.
		std::stringstream ss;
		ss << std::hex << std::setfill('0') << std::setw(16) << bytes;
		return ss.str();
	}

	uint64_t HexaToUint64(const std::string hex)
	{
		uint64_t id;
		sscanf_s(hex.c_str(), "%16llx", &id);
		return id;
	}
}
