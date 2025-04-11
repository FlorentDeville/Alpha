/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Systems/Objects/Object.h"

#include "Systems/Reflection/ReflectionMacro.h"

namespace Systems
{
	//Base class for any asset
	ENABLE_REFLECTION_WITH_NS(Systems, AssetObject)
	class AssetObject : public Object
	{

		START_REFLECTION(Systems::AssetObject)
			ADD_BASETYPE(Systems::Object)
		END_REFLECTION()
	};
}
