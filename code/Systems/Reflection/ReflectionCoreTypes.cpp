/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Systems/Reflection/ReflectionCoreTypes.h"

#include "Systems/Reflection/ReflectionMacro.h"

namespace Systems
{
	void RegisterCoreTypesToReflection()
	{
		REGISTER_TYPE(Core::Color);
		REGISTER_TYPE(Core::Guid);
		REGISTER_TYPE(Core::Vec4f);
		REGISTER_TYPE(Core::Mat44f);
		REGISTER_TYPE(Core::Sid);
	}
}
