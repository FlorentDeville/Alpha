/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Core/Reflection/ReflectionMacro.h"

namespace Systems
{
	//I use BM_ as a prefix because in dx12 includes wingdi.h which defines a macro OPAQUE and messes up with my enum.
	//Once dx12 is hidden from the rest, I can remove the prefix.
	enum BlendMode
	{
		BM_OPAQUE,
		BM_TRANSLUCENT
	};
}

ENABLE_ENUM_REFLECTION(Systems, BlendMode)

START_ENUM_REFLECTION(Systems, BlendMode)
	ADD_ENUM_ENTRY(BM_OPAQUE)
	ADD_ENUM_ENTRY(BM_TRANSLUCENT)
END_ENUM_REFLECTION()
