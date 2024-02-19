/********************************************************************/
/* © 2024 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Systems/ComponentPool.h"

namespace Systems
{
	ComponentPool::ComponentPool()
		: m_elementSize(0)
		, m_pData(nullptr)
	{}

	ComponentPool::~ComponentPool()
	{
		delete[] m_pData;
	}

	void ComponentPool::Init(size_t elementSize)
	{
		m_elementSize = elementSize;
		m_pData = new char[elementSize * MAX_ENTITIES];
	}

	void* ComponentPool::Get(size_t index)
	{
		return m_pData + (index * m_elementSize);
	}
}
