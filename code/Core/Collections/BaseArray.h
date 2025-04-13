/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include <cstdint>

namespace Core
{
	class BaseArray
	{
	public:
		BaseArray() = default;
		virtual ~BaseArray() = default;

		virtual int32_t GetSize() const = 0;
		virtual void* GetElement(int32_t index) = 0;
		virtual const void* GetConstElement(int32_t) const = 0;

		virtual void Resize(int32_t newSize) = 0;
	};
}
