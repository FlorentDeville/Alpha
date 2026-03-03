/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Systems/Reflection/ReflectionMacro.h"

DEFINE_TYPE_RESOLVER(uint8_t)
DEFINE_TYPE_RESOLVER(int8_t)
DEFINE_TYPE_RESOLVER(int32_t)
DEFINE_TYPE_RESOLVER(uint32_t)
DEFINE_TYPE_RESOLVER(uint16_t)
DEFINE_TYPE_RESOLVER(double)
DEFINE_TYPE_RESOLVER(float)
DEFINE_TYPE_RESOLVER(bool)
DEFINE_TYPE_RESOLVER(char)
DEFINE_TYPE_RESOLVER(std::string)

namespace Systems
{
	// Register the standard c++ types like bool, float ,etc
	void RegisterStandardTypesToReflection();
}
