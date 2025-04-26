/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include <cstdint>

namespace Core
{
	class MemoryPool
	{
	public:
		MemoryPool(size_t size, uint16_t alignment);
		~MemoryPool();

		void Allocate();
		void Free();

		void* GetStartPtr() const;
		size_t GetSize() const;
		uint16_t GetAlignment() const;

	private:
		void* m_pStart;
		size_t m_size;
		uint16_t m_alignment;
	};
}
