/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Core/Sid/Sid.h"

#include <map>

namespace Core
{
	class MemoryPool;

	//extern std::map<Sid, MemoryPool> g_PoolMap;

	class MemoryPoolMap
	{
	public:
		static void AllocateAllPools();
		static void FreeAllPools();

		static const MemoryPool& GetPool(Core::Sid sid);
	};
}
