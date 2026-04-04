/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Rendering/Internal/BlendFactorToDx12.h"

#include <assert.h>

namespace Rendering
{
	namespace Internal
	{
		D3D12_BLEND GetDx12BlendFactor(BlendFactor factor)
		{
			switch (factor)
			{
			case BlendFactor::ZERO:
				return D3D12_BLEND_ZERO;
				break;

			case BlendFactor::ONE:
				return D3D12_BLEND_ONE;
				break;

			case BlendFactor::FACTOR:
				return D3D12_BLEND_BLEND_FACTOR;
				break;

			case BlendFactor::INV_FACTOR:
				return D3D12_BLEND_INV_BLEND_FACTOR;
				break;

			default:
				assert(false && "Unknown blend factor");
				break;
			}

			return D3D12_BLEND_ZERO;
		}
	}
}