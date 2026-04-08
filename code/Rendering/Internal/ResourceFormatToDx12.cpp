/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Rendering/Internal/ResourceFormatToDx12.h"

#include <assert.h>

namespace Rendering
{
	namespace Internal
	{
		DXGI_FORMAT GetDx12ResourceFormat(BufferFormat format)
		{
			switch (format)
			{
			case BufferFormat::R32_FLOAT:
				return DXGI_FORMAT_R32_FLOAT;
				break;

			case BufferFormat::R8G8B8A8_UNORM_SRGB:
				return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
				break;

			case BufferFormat::R8G8B8A8_UINT:
				return DXGI_FORMAT_R8G8B8A8_UINT;
				break;

			case BufferFormat::R16G16B16A16_FLOAT:
				return DXGI_FORMAT_R16G16B16A16_FLOAT;
				break;

			case BufferFormat::R16_UINT:
				return DXGI_FORMAT_R16_UINT;
				break;

			default:
				assert(false && "Unknown resource format");
				break;
			}

			return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		}
	}
}