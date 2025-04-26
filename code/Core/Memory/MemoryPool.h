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
		MemoryPool();
		MemoryPool(size_t size);
		~MemoryPool();

		void Allocate();
		void Free();

		void* GetStartPtr() const;
		size_t GetSize() const;

	private:
		void* m_pStart;
		size_t m_size;
	};
}
