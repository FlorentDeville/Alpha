/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Rendering/Internal/DepthWriteMaskToDx12.h"

#include <assert.h>

namespace Rendering
{
	namespace Internal
	{
		D3D12_DEPTH_WRITE_MASK GetDx12DepthWriteMask(DepthWriteMask mask)
		{
			switch (mask)
			{
			case DepthWriteMask::ZERO:
				return D3D12_DEPTH_WRITE_MASK_ZERO;
				break;

			case DepthWriteMask::ALL:
				return D3D12_DEPTH_WRITE_MASK_ALL;
				break;
			}

			assert(false && "Unknown mask");

			return D3D12_DEPTH_WRITE_MASK_ALL;
		}
	}
}
