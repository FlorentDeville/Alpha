/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Systems/Reflection/ReflectionMacro.h"

DEFINE_GET_TYPENAME(int32_t)
DEFINE_GET_TYPENAME(double)
DEFINE_GET_TYPENAME(float)
DEFINE_GET_TYPENAME(bool)

namespace Systems
{
	// Register the standard c++ types like bool, float ,etc
	void RegisterStandardTypesToReflection();
}
