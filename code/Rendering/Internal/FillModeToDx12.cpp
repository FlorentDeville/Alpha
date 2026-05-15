/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Rendering/Internal/FillModeToDx12.h"

#include <assert.h>

namespace Rendering
{
	namespace Internal
	{
		D3D12_FILL_MODE GetDx12FillMode(FillMode mask)
		{
			switch (mask)
			{
			case FillMode::SOLID:
				return D3D12_FILL_MODE_SOLID;
				break;

			case FillMode::WIREFRAME:
				return D3D12_FILL_MODE_WIREFRAME;
				break;
			}

			assert(false && "Unknown mask");

			return D3D12_FILL_MODE_WIREFRAME;
		}
	}
}
