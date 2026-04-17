/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Rendering/PipelineState/DepthWriteMask.h"

#include <d3d12.h>

namespace Rendering
{
	namespace Internal
	{
		D3D12_DEPTH_WRITE_MASK GetDx12DepthWriteMask(DepthWriteMask mask);
	}
}
