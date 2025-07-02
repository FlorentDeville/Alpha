/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Rendering/ConstantBuffer/ConstantBufferPool.h"

namespace Rendering
{
	class LinearConstantBufferPool : public ConstantBufferPool
	{
	public:
		LinearConstantBufferPool();
		~LinearConstantBufferPool();

		void FreeAll();

		int GetFreeConstantBufferIndex();

	private:
		//Next free constant buffer to use
		int m_current;
	};
}
