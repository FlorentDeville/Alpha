/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include <cstdint>

namespace Core
{
	// Base class for all arrays class. This base class is not a teplate so I can use it with the reflection.
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

		//Create a new element at the end of the array
		virtual void AddElement() = 0;
	};
}
