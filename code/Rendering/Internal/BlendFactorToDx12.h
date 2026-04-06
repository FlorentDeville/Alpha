/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Rendering/PipelineState/BlendFactor.h"

#include <d3d12.h>

namespace Rendering
{
	namespace Internal
	{
		D3D12_BLEND GetDx12BlendFactor(BlendFactor factor);
	}
}