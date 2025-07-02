/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Rendering/ConstantBuffer/LinearConstantBufferPool.h"

#include <assert.h>

namespace Rendering
{
	LinearConstantBufferPool::LinearConstantBufferPool()
		: ConstantBufferPool()
		, m_current(0)
	{ }

	LinearConstantBufferPool::~LinearConstantBufferPool()
	{ }

	void LinearConstantBufferPool::FreeAll()
	{
		m_current = 0;
	}

	int LinearConstantBufferPool::GetFreeConstantBufferIndex()
	{
		assert(m_current < m_elementCount);

		int ret = m_current;
		++m_current;
		return ret;
	}
}
