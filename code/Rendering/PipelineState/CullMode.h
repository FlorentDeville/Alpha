/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Core/Reflection/ReflectionMacro.h"

namespace Rendering
{
	enum CullMode
	{
		NONE,
		FRONT,
		BACK
	};
}

ENABLE_ENUM_REFLECTION(Rendering, CullMode)

START_ENUM_REFLECTION(Rendering, CullMode)
	ADD_ENUM_ENTRY(NONE)
	ADD_ENUM_ENTRY(FRONT)
	ADD_ENUM_ENTRY(BACK)
END_ENUM_REFLECTION()
