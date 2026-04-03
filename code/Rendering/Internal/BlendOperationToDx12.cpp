/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Rendering/Internal/BlendOperationToDx12.h"

#include <assert.h>

namespace Rendering
{
	namespace Internal
	{
		D3D12_BLEND_OP GetDx12BlendOperation(BlendOperation op)
		{
			switch (op)
			{
			case BlendOperation::ADD:
				return D3D12_BLEND_OP_ADD;
				break;

			case BlendOperation::SUBSTRACT:
				return D3D12_BLEND_OP_SUBTRACT;
				break;

			case BlendOperation::REV_SUBSTRACT:
				return D3D12_BLEND_OP_REV_SUBTRACT;
				break;

			case BlendOperation::MIN:
				return D3D12_BLEND_OP_MIN;
				break;

			case BlendOperation::MAX:
				return D3D12_BLEND_OP_MAX;
				break;

			default:
				assert(false && "Unknown blend operation");
				break;
			}

			return D3D12_BLEND_OP_ADD;
		}
	}
}