/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Core/Memory/Allocator.h"

namespace Core
{
	class LinearAllocator : public Allocator
	{
	public:
		LinearAllocator(void* pStart, size_t size);
		~LinearAllocator();

		void* Allocate(size_t size, uint16_t alignment) override;

		//Doesn't do anything for a linear allocator. You can only release the entire memory using Clear.
		void Free(void* pPtr) override;

		// Free the entire pool
		void Clear();

	private:
		char* m_pTop; //current top pointer for the stack
	};
}
