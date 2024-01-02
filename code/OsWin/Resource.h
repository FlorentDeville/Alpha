/********************************************************************/
/* © 2024 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include <cstdint>

namespace Os
{
	class Resource
	{
	public:
		static bool GetResource(int16_t resourceId, const char* resourceType, char** ptr, uint32_t& size);
	};
}
