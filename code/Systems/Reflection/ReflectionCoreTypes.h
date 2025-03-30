/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Core/Math/Mat44f.h"
#include "Core/Math/Vec4f.h"

#include "Systems/Reflection/ReflectionMacro.h"

namespace Core
{
	class Vec4f;
	class Mat44f;
}

DEFINE_GET_TYPENAME(Core::Vec4f)
DEFINE_GET_TYPENAME(Core::Mat44f)

namespace Systems
{
	void RegisterCoreTypesToReflection();
}
