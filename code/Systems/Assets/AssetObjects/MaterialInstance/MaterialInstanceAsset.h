/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Core/Collections/Array.h"

#include "Systems/Assets/AssetObjects/Material/MaterialAsset.h"
#include "Systems/Assets/AssetObjects/Material/MaterialParameterDescription.h"
#include "Systems/Assets/NewAssetId.h"
#include "Systems/Objects/AssetObject.h"

ENABLE_REFLECTION(Systems, MaterialInstanceAsset)

namespace Systems
{
	class MaterialInstanceAsset : public AssetObject
	{
	public:
		MaterialInstanceAsset();
		~MaterialInstanceAsset();

		bool InitialiseFromBaseMaterial(MaterialAsset* pBaseMaterial);

		bool Refresh();

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

		START_REFLECTION(Systems::MaterialInstanceAsset)
			ADD_BASETYPE(Systems::AssetObject)
			ADD_FIELD(m_material)
			ADD_FIELD(m_perMaterialParameters)
			ADD_FIELD(m_texturesBindingInfo)
		END_REFLECTION()
	};
}
