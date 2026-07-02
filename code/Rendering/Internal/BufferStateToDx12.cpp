/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Rendering/Internal/BufferStateToDx12.h"

#include <assert.h>

namespace Rendering
{
	namespace Internal
	{
		D3D12_RESOURCE_STATES GetDx12BufferState(BufferState format)
		{
			switch (format)
			{
			case BufferState::RESOLVE_DEST:
				return D3D12_RESOURCE_STATE_RESOLVE_DEST;
				break;

			case BufferState::RESOLVE_SOURCE:
				return D3D12_RESOURCE_STATE_RESOLVE_SOURCE;
				break;

			default:
				assert(false && "Unknown buffer state");
				break;
			}

			return D3D12_RESOURCE_STATE_COMMON;
		}
	}
}
