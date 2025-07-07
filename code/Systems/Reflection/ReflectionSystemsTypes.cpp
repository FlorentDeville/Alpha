/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Systems/Reflection/ReflectionSystemsTypes.h"

#include "Systems/Assets/AssetObjects/Material/MaterialAsset.h"
#include "Systems/Assets/AssetObjects/Material/MaterialCBufferBindingInfo.h"
#include "Systems/Assets/AssetObjects/Material/MaterialParameterDescription.h"
#include "Systems/Assets/AssetObjects/MaterialInstance/MaterialInstanceAsset.h"
#include "Systems/Assets/AssetObjects/MeshAsset.h"
#include "Systems/GameComponent/StaticMeshComponent.h"
#include "Systems/GameComponent/TransformComponent.h"
#include "Systems/Reflection/ReflectionMacro.h"
#include "Systems/Objects/AssetObject.h"
#include "Systems/Objects/GameComponent.h"
#include "Systems/Objects/GameObject.h"
#include "Systems/Objects/Object.h"

namespace Systems
{
	void RegisterSystemsTypesToReflection()
	{
		REGISTER_TYPE(Systems::AssetId);
		REGISTER_TYPE(Systems::NewAssetId);

		Systems::Object::RegisterReflection();
		Systems::GameObject::RegisterReflection();
		Systems::GameComponent::RegisterReflection();
		Systems::StaticMeshComponent::RegisterReflection();
		Systems::TransformComponent::RegisterReflection();

		Systems::AssetObject::RegisterReflection();
		Systems::MaterialAsset_v1::RegisterReflection();
		Systems::MeshAsset_v1::RegisterReflection();
		Systems::MeshAsset_v2::RegisterReflection();
		Systems::MaterialParameterDescription::RegisterReflection();
		Systems::MaterialCBufferBindingInfo::RegisterReflection();
		Systems::MaterialInstanceAsset::RegisterReflection();
	}
}
