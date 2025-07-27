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

		virtual uint32_t GetSize() const = 0;
		virtual void* GetElement(uint32_t index) = 0;
		virtual const void* GetConstElement(uint32_t) const = 0;

		virtual void RemoveElement(uint32_t index) = 0;

		virtual void Resize(uint32_t newSize) = 0;
	};
}
