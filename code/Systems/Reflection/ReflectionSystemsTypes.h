/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Systems/Assets/NewAssetId.h"
#include "Systems/Reflection/ReflectionMacro.h"

DEFINE_TYPE_RESOLVER(Systems::NewAssetId)

namespace Systems
{
	void RegisterSystemsTypesToReflection();
}
