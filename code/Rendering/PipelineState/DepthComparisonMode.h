/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Core/Reflection/ReflectionMacro.h"

namespace Rendering
{
	enum DepthComparisonMode
	{
		NEVER,
		LESS,
		LESS_OR_EQUAL,
		GREATER,
		GREATER_OR_EQUAL,
		EQUAL,
		NOT_EQUAL,
		ALWAYS
	};
}

ENABLE_ENUM_REFLECTION(Rendering, DepthComparisonMode)

START_ENUM_REFLECTION(Rendering, DepthComparisonMode)
	ADD_ENUM_ENTRY(NEVER)
	ADD_ENUM_ENTRY(LESS)
	ADD_ENUM_ENTRY(LESS_OR_EQUAL)
	ADD_ENUM_ENTRY(GREATER)
	ADD_ENUM_ENTRY(GREATER_OR_EQUAL)
	ADD_ENUM_ENTRY(EQUAL)
	ADD_ENUM_ENTRY(NOT_EQUAL)
	ADD_ENUM_ENTRY(ALWAYS)
END_ENUM_REFLECTION()
