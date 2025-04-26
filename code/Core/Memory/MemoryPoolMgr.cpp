/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Core/Memory/MemoryPoolMgr.h"

#include "Core/Memory/MemoryPool.h"

#define KB * 1024
#define MB KB * 1024
#define GB MB * 1024

#define DEFAULT_ALIGNMENT 2

namespace Core
{
	MemoryPoolMgr::MemoryPoolMgr()
		: m_pools()
	{ }

	MemoryPoolMgr::~MemoryPoolMgr()
	{
		for (std::pair<const Sid,MemoryPool*>& pair : m_pools)
			delete pair.second;

		m_pools.clear();
	}

	void MemoryPoolMgr::Init()
	{
		m_pools[SID("Root")] = new MemoryPool(1 GB, DEFAULT_ALIGNMENT);


		m_pools[SID("Json")] = new MemoryPool(1 KB, DEFAULT_ALIGNMENT);
	}

	void MemoryPoolMgr::Shutdown()
	{ }

	void MemoryPoolMgr::AllocateAllPools()
	{
		for (std::pair<const Sid, MemoryPool*>& pair : m_pools)
			pair.second->Allocate();
	}

	void MemoryPoolMgr::FreeAllPools()
	{
		for (std::pair<const Sid, MemoryPool*>& pair : m_pools)
			pair.second->Free();
	}

	MemoryPool* MemoryPoolMgr::GetPool(Core::Sid sid) const
	{
		std::map<Sid, MemoryPool*>::const_iterator it = m_pools.find(sid);
		if (it == m_pools.cend())
			return nullptr;

		return it->second;
	}
}
