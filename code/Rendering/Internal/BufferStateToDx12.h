/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Rendering/BufferState.h"

#include <d3d12.h>

namespace Rendering
{
	namespace Internal
	{
		D3D12_RESOURCE_STATES GetDx12BufferState(BufferState format);
	}
}
