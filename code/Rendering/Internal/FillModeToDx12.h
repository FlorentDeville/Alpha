/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Rendering/PipelineState/FillMode.h"

#include <d3d12.h>

namespace Rendering
{
	namespace Internal
	{
		D3D12_FILL_MODE GetDx12FillMode(FillMode mask);
	}
}
