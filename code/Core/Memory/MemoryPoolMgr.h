/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Core/Sid/Sid.h"
#include "Core/Singleton.h"

#include <map>

namespace Core
{
	class MemoryPool;

	class MemoryPoolMgr : public Singleton<MemoryPoolMgr>
	{
	public:
		MemoryPoolMgr();
		~MemoryPoolMgr();

		void Init() override;
		void Shutdown() override;

		void AllocateAllPools();
		void FreeAllPools();

		MemoryPool* GetPool(Core::Sid sid) const;

	private:
		std::map<Core::Sid, MemoryPool*> m_pools;
	};
}
