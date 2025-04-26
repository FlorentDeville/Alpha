/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

namespace Core
{
	class Allocator;

	class AllocatorJanitor
	{
	public:
		AllocatorJanitor(Allocator* pAllocator);
		~AllocatorJanitor();
	};
}
