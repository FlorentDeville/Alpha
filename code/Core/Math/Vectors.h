/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include <cstdint>

namespace Core
{
	class UInt2
	{
	public:
		uint32_t x;
		uint32_t y;

		UInt2()
			: x(0)
			, y(0)
		{ }

		UInt2(uint32_t _x, uint32_t _y)
			: x(_x)
			, y(_y)
		{ }

		bool operator==(const UInt2& other) const
		{
			return x == other.x && y == other.y;
		}

		bool operator!=(const UInt2& other) const
		{
			return !(*this == other);
		}
	};

	class Int2
	{
	public:
		int32_t x;
		int32_t y;

		Int2()
			: x(0)
			, y(0)
		{ }

		Int2(int32_t _x, int32_t _y)
			: x(_x)
			, y(_y)
		{ }
	};

	class Float2
	{
	public:
		float x;
		float y;

		Float2()
			: x(0)
			, y(0)
		{ }

		Float2(float _x, float _y)
			: x(_x)
			, y(_y)
		{ }
	};


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
