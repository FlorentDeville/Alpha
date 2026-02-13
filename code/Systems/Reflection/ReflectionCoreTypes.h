/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Core/Collections/Array.h"
#include "Core/Color/Color.h"
#include "Core/Guid/Guid.h"
#include "Core/Math/Mat44f.h"
#include "Core/Math/Vec4f.h"
#include "Core/Sid/Sid.h"

#include "Systems/Reflection/ReflectionMacro.h"

namespace Systems
{
	DEFINE_TYPE_RESOLVER(Core::Color)
	DEFINE_TYPE_RESOLVER(Core::Guid)
	DEFINE_TYPE_RESOLVER(Core::Vec4f)
	DEFINE_TYPE_RESOLVER(Core::Mat44f)
	DEFINE_TYPE_RESOLVER(Core::Sid)

	void RegisterCoreTypesToReflection();
}
