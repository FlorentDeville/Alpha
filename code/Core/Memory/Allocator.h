/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include <cstdint>

namespace Core
{
	class Allocator
	{
	public:
		Allocator();
		Allocator(void* pStart, size_t size);
		virtual ~Allocator();

		virtual void Init(void* pStart, size_t size);

		virtual void* Allocate(size_t size, uint16_t alignment) = 0;
		virtual void Free(void* pPtr) = 0;

	protected:
		uint16_t ComputeAlignmentOffset(const void* pPtr, uint16_t alignement) const;

		char* m_pStart; //start pointer of the memory usable by this allocator
		size_t m_size; //size of the memory usable by this allocator
	};
}
