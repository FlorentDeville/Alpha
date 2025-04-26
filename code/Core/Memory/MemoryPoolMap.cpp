/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Core/Memory/MemoryPoolMap.h"

#include "Core/Memory/MemoryPool.h"

#define KB * 1024
#define MB KB * 1024
#define GB MB * 1024

namespace Core
{
	std::map<Core::Sid, MemoryPool> CreatePoolMap()
	{
		std::map<Core::Sid, MemoryPool> map;
		
		map[SID("Root")] = MemoryPool(1 GB);
		map[SID("Json")] = MemoryPool(1 GB);
		
		return map;
	}

	//This is the map of pools used in the entire engine. Keep it global so it is initialized before main is called.
	std::map<Core::Sid, MemoryPool> g_PoolMap = CreatePoolMap();

	void MemoryPoolMap::AllocateAllPools()
	{
		for (std::pair<const Core::Sid, MemoryPool>& pair : g_PoolMap)
		{
			pair.second.Allocate();
		}
	}

	void MemoryPoolMap::FreeAllPools()
	{
		for (std::pair<const Core::Sid, MemoryPool>& pair : g_PoolMap)
		{
			pair.second.Free();
		}
	}

	const MemoryPool& MemoryPoolMap::GetPool(Core::Sid sid)
	{
		return g_PoolMap[sid];
	}
}
