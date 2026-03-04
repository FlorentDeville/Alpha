/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Core/Reflection/ReflectionMacro.h"

namespace Rendering
{
	enum CullMode
	{
		None,
		Front,
		Back
	};
}

ENABLE_ENUM_REFLECTION(Rendering, CullMode)

START_ENUM_REFLECTION(Rendering, CullMode)
	ADD_ENUM_ENTRY(None)
	ADD_ENUM_ENTRY(Front)
	ADD_ENUM_ENTRY(Back)
END_ENUM_REFLECTION()
