/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Core/Memory/LinearAllocator.h"

namespace Core
{
	LinearAllocator::LinearAllocator()
		: Allocator()
		, m_pTop(nullptr)
	{ }

	LinearAllocator::LinearAllocator(void* pStart, size_t size)
		: Allocator(pStart, size)
		, m_pTop(reinterpret_cast<char*>(pStart))
	{ }

	LinearAllocator::~LinearAllocator()
	{ }

	void LinearAllocator::Init(void* pStart, size_t size)
	{
		Allocator::Init(pStart, size);
		m_pTop = reinterpret_cast<char*>(pStart);
	}

	void* LinearAllocator::Allocate(size_t size, uint16_t alignment)
	{
		uint16_t offset = ComputeAlignmentOffset(m_pTop, alignment);

		if (m_pTop + offset + size >= m_pStart + m_size)
			return nullptr;

		char* newPtr = m_pTop + offset;
		m_pTop = newPtr + size;

		return newPtr;
	}

	void LinearAllocator::Free(void* pPtr)
	{ }

	void LinearAllocator::Clear()
	{
		m_pTop = m_pStart;
	}
}
