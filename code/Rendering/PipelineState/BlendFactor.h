/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

namespace Rendering
{
	enum class BlendFactor
	{
		ZERO,			// blend factor of (0, 0, 0, 0)
		ONE,			// blend factor of (1, 1, 1, 1)
		FACTOR,			// use the blend factor set with SetBlendFactor
		INV_FACTOR,		// use the inverse blend fator set with SetBlendFactor (1 - BlendFactor)

		COUNT
	};
}
