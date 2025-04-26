/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Core/Memory/MemoryPool.h"

#include <cstdlib>

namespace Core
{
	MemoryPool::MemoryPool()
		: m_pStart(nullptr)
		, m_size(0)
	{ }

	MemoryPool::MemoryPool(size_t size)
		: m_pStart(nullptr)
		, m_size(size)
	{ }

	MemoryPool::~MemoryPool()
	{ }

	void MemoryPool::Allocate()
	{
		m_pStart = malloc(m_size);
	}

	void MemoryPool::Free()
	{
		free(m_pStart);
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
}
