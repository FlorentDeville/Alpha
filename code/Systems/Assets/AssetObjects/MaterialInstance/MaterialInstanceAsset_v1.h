/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Core/Collections/Array.h"

#include "Systems/Assets/AssetObjects/Material/MaterialAsset.h"
#include "Systems/Assets/AssetObjects/Material/MaterialParameterDescription.h"
#include "Systems/Assets/NewAssetId.h"
#include "Systems/Objects/AssetObject.h"

namespace Systems
{
	ENABLE_REFLECTION_WITH_NS(Systems, MaterialInstanceAsset_v1)
	class MaterialInstanceAsset_v1 : public AssetObject
	{
	public:
		MaterialInstanceAsset_v1();
		~MaterialInstanceAsset_v1();

		bool InitialiseFromBaseMaterial(MaterialAsset& baseMaterial);

		static const std::string& GetAssetTypeName();
		static Core::Sid GetAssetTypeNameSid();

	private:
		NewAssetId m_material;
		Core::Array<MaterialParameterDescription> m_perMaterialParameters;

		START_REFLECTION(Systems::MaterialInstanceAsset_v1)
			ADD_BASETYPE(Systems::AssetObject)
			ADD_FIELD(m_material)
			ADD_FIELD(m_perMaterialParameters)
		END_REFLECTION()
	};
}
