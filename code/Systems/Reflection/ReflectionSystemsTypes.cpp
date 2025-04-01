/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Systems/Reflection/ReflectionSystemsTypes.h"

#include "Systems/Reflection/ReflectionMacro.h"
#include "Systems/Objects/GameObject.h"
#include "Systems/Objects/Object.h"

namespace Systems
{
	void RegisterSystemsTypesToReflection()
	{
		Systems::Object::RegisterReflection();
		Systems::GameObject::RegisterReflection();
	}
}
