/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Core/Singleton.h"

#include <stack>

namespace Core
{
	class Allocator;

	class AllocatorStack : public Singleton<AllocatorStack>
	{
	public:
		AllocatorStack();
		~AllocatorStack();

		void Init() override;
		void Shutdown() override;

		void Push(Allocator* pAllocator);
		void Pop();

	private:
		std::stack<Allocator*> m_stack;
	};
}
