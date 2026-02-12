/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Systems/Assets/NewAssetId.h"
#include "Systems/Reflection/ReflectionMacro.h"

namespace Systems
{
	DEFINE_TYPE_RESOLVER(Systems::NewAssetId)

	void RegisterSystemsTypesToReflection();
}
