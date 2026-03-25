/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include <cstdint>

namespace Systems
{
	enum class RenderView : uint8_t
	{
		Game = 0x1,
		ShadowMap = 0x2,
		ObjectId = 0x4
	};

	inline RenderView operator|(RenderView a, RenderView b)
	{
		return static_cast<RenderView>(static_cast<uint8_t>(a) | static_cast<uint8_t>(b));
	}

	inline bool operator&(RenderView a, RenderView b)
	{
		return (static_cast<uint8_t>(a) & static_cast<uint8_t>(b)) != 0;
	}
}
