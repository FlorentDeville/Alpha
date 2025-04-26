/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Core/Memory/MemoryPool.h"

#include <cstdlib>

namespace Core
{
	MemoryPool::MemoryPool(size_t size, uint16_t alignment)
		: m_pStart(nullptr)
		, m_size(size)
		, m_alignment(alignment)
	{ }

	MemoryPool::~MemoryPool()
	{ }

	void MemoryPool::Allocate()
	{
		m_pStart = _aligned_malloc(m_size, m_alignment);
	}

	void MemoryPool::Free()
	{
		_aligned_free(m_pStart);
		m_pStart = nullptr;
	}

	void* MemoryPool::GetStartPtr() const
	{
		return m_pStart;
	}

	size_t MemoryPool::GetSize() const
	{
		return m_size;
	}

	uint16_t MemoryPool::GetAlignment() const
	{
		return m_alignment;
	}
}
