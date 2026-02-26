/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Systems/Reflection/ReflectionSystemsTypes.h"

#include "Systems/Assets/AssetObjects/Level/LevelAsset_v1.h"
#include "Systems/Assets/AssetObjects/Material/MaterialAsset_v1.h"
#include "Systems/Assets/AssetObjects/Material/MaterialAsset_v2.h"
#include "Systems/Assets/AssetObjects/Material/MaterialCBufferBindingInfo.h"
#include "Systems/Assets/AssetObjects/Material/MaterialParameterDescription.h"
#include "Systems/Assets/AssetObjects/Material/TextureBindingInfo.h"
#include "Systems/Assets/AssetObjects/MaterialInstance/MaterialInstanceAsset.h"
#include "Systems/Assets/AssetObjects/Mesh/MeshAsset_v1.h"
#include "Systems/Assets/AssetObjects/Mesh/MeshAsset_v2.h"
#include "Systems/Assets/AssetObjects/Texture/TextureAsset_v1.h"
#include "Systems/GameComponent/Lights/DirectionalLightComponent.h"
#include "Systems/GameComponent/Lights/PointLightComponent.h"
#include "Systems/GameComponent/Lights/SpotLightComponent.h"
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
		REGISTER_TYPE(Systems::NewAssetId);

		Systems::Object::RegisterReflection();
		Systems::GameObject::RegisterReflection();
		Systems::GameComponent::RegisterReflection();
		Systems::StaticMeshComponent::RegisterReflection();
		Systems::TransformComponent::RegisterReflection();
		Systems::DirectionalLightComponent::RegisterReflection();
		Systems::PointLightComponent::RegisterReflection();
		Systems::SpotLightComponent::RegisterReflection();

		Systems::AssetObject::RegisterReflection();
		Systems::LevelAsset_v1::RegisterReflection();
		Systems::MaterialAsset_v1::RegisterReflection();
		Systems::MaterialAsset_v2::RegisterReflection();
		Systems::MeshAsset_v1::RegisterReflection();
		Systems::MeshAsset_v2::RegisterReflection();
		Systems::MaterialParameterDescription::RegisterReflection();
		Systems::MaterialCBufferBindingInfo::RegisterReflection();
		Systems::MaterialInstanceAsset::RegisterReflection();
		Systems::TextureAsset_v1::RegisterReflection();
		Systems::TextureBindingInfo::RegisterReflection();
	}
}
