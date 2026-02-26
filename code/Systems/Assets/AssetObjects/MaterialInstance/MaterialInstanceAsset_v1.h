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

		bool InitialiseFromBaseMaterial(MaterialAsset* pBaseMaterial);

		NewAssetId GetBaseMaterialId() const;

		const Systems::MaterialAsset* GetBaseMaterial() const;

		const Core::Array<MaterialParameterDescription>& GetMaterialParameterDescription() const;
		const Core::Array<TextureBindingInfo>& GetTexturesBindingInfo() const;

		static const std::string& GetAssetTypeName();
		static Core::Sid GetAssetTypeNameSid();

	private:
		NewAssetId m_material;
		Core::Array<MaterialParameterDescription> m_perMaterialParameters;
		Core::Array<TextureBindingInfo> m_texturesBindingInfo;

		//Non-reflected data

		//Cached pointer to the base material
		mutable Systems::MaterialAsset* m_pBaseMaterial;

		START_REFLECTION(Systems::MaterialInstanceAsset_v1)
			ADD_BASETYPE(Systems::AssetObject)
			ADD_FIELD(m_material)
			ADD_FIELD(m_perMaterialParameters)
			ADD_FIELD(m_texturesBindingInfo)
		END_REFLECTION()
	};
}
