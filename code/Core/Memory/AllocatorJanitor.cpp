/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Core/Memory/AllocatorJanitor.h"

#include "Core/Memory/AllocatorStack.h"

namespace Core
{
	AllocatorJanitor::AllocatorJanitor(Allocator* pAllocator)
	{
		AllocatorStack::Get().Push(pAllocator);
	}

	AllocatorJanitor::~AllocatorJanitor()
	{
		AllocatorStack::Get().Pop();
	}
}
