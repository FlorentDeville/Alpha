/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

namespace Rendering
{
	enum class BlendOperation
	{
		ADD,
		SUBSTRACT,		//render target value - pixel shader value
		REV_SUBSTRACT,	//pixel shader value - render target value
		MIN,
		MAX,

		COUNT
	};
}