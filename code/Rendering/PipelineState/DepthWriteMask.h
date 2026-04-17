/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

namespace Rendering
{
	enum class DepthWriteMask
	{
		ZERO,			// don't write to the depth stencil buffer
		ALL,			// enable writing to the depth stencil buffer
		
		COUNT
	};
}
