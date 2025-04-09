/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Systems/Reflection/ReflectionSystemsTypes.h"

#include "Systems/GameComponent/StaticMeshComponent.h"
#include "Systems/GameComponent/TransformComponent.h"
#include "Systems/Reflection/ReflectionMacro.h"
#include "Systems/Objects/GameComponent.h"
#include "Systems/Objects/GameObject.h"
#include "Systems/Objects/Object.h"


namespace Systems
{
	void RegisterSystemsTypesToReflection()
	{
		REGISTER_TYPE(Systems::AssetId);

		Systems::Object::RegisterReflection();
		Systems::GameObject::RegisterReflection();
		Systems::GameComponent::RegisterReflection();
		Systems::StaticMeshComponent::RegisterReflection();
		Systems::TransformComponent::RegisterReflection();
	}
}
