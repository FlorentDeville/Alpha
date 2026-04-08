/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include <d3d12.h>
#include <cstdint>

namespace Rendering
{
	namespace Internal
	{
		class Dx12Buffer
		{
		public:
			Dx12Buffer()
				: m_pResource(nullptr)
				, m_generation(0)
				, m_isUsed(false)
			{ }

			ID3D12Resource* m_pResource;
			uint8_t m_generation;
			bool m_isUsed;
		};
	}
}
