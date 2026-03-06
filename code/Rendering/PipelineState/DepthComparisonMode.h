/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Core/Reflection/ReflectionMacro.h"

namespace Rendering
{
	enum DepthComparisonMode
	{
		Never,
		Less,
		LessOrEqual,
		Greater,
		GreaterOrEqual,
		Equal,
		NotEqual,
		Always
	};
}

ENABLE_ENUM_REFLECTION(Rendering, DepthComparisonMode)

START_ENUM_REFLECTION(Rendering, DepthComparisonMode)
	ADD_ENUM_ENTRY(Never)
	ADD_ENUM_ENTRY(Less)
	ADD_ENUM_ENTRY(LessOrEqual)
	ADD_ENUM_ENTRY(Greater)
	ADD_ENUM_ENTRY(GreaterOrEqual)
	ADD_ENUM_ENTRY(Equal)
	ADD_ENUM_ENTRY(NotEqual)
	ADD_ENUM_ENTRY(Always)
END_ENUM_REFLECTION()
