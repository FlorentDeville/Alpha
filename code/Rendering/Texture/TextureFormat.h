/********************************************************************/
/* © 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

namespace Rendering
{
	enum class TextureFormat
	{
		Unknown = -1,
		BC1_SRGB = 0,			//DXGI_FORMAT_BC1_UNORM_SRGB
		BC7_SRGB = 1,			//DXGI_FORMAT_BC7_UNORM_SRGB

		//You can add a new enum value anywhere inside the enum but the value needs to be set
		//explicitly. Once a value is set you cannot change it.

		Count
	};
}