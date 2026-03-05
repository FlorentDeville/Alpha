/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Core/Reflection/ReflectionMacro.h"

namespace Systems
{
	enum TextureType
	{
		Texture2D,
		Cubemap
	};
}

ENABLE_ENUM_REFLECTION(Systems, TextureType)

START_ENUM_REFLECTION(Systems, TextureType)
	ADD_ENUM_ENTRY(Texture2D)
	ADD_ENUM_ENTRY(Cubemap)
END_ENUM_REFLECTION()
