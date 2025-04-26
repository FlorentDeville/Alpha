/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Core/Memory/AllocatorStack.h"

namespace Core
{
	AllocatorStack::AllocatorStack()
		: m_stack()
	{ }

	AllocatorStack::~AllocatorStack()
	{ }

	void AllocatorStack::Init()
	{ }

	void AllocatorStack::Shutdown()
	{ }

	void AllocatorStack::Push(Allocator* pAllocator)
	{
		m_stack.push(pAllocator);
	}

	void AllocatorStack::Pop()
	{
		m_stack.pop();
	}
}
