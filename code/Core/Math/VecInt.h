/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include <cstdint>

namespace Core
{
	class Int3
	{
	public:
		int32_t x;
		int32_t y;
		int32_t z;

		Int3()
			: x(0)
			, y(0)
			, z(0)
		{ }

		Int3(int32_t _x, int32_t _y, int32_t _z)
			: x(_x)
			, y(_y)
			, z(_z)
		{ }

	};
}
