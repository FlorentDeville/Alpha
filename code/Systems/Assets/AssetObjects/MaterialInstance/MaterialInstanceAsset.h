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
		enum class RefreshResult
		{
			Success,
			Failure,
			UpToDate,

			Count
		};

		MaterialInstanceAsset();
		~MaterialInstanceAsset();

		bool InitialiseFromBaseMaterial(MaterialAsset* pBaseMaterial);

		RefreshResult Refresh();

		NewAssetId GetBaseMaterialId() const;

		const Systems::MaterialAsset* GetBaseMaterial() const;

		const Core::Array<MaterialParameterDescription>& GetMaterialParameterDescription() const;
		const Core::Array<TextureBindingInfo>& GetTexturesBindingInfo() const;

		Core::Sid GetParametersSchemaHash() const;
		void CalculateParametersSchemaHash();

		static const std::string& GetAssetTypeName();
		static Core::Sid GetAssetTypeNameSid();

	private:
		NewAssetId m_material;
		Core::Array<MaterialParameterDescription> m_perMaterialParameters;
		Core::Array<TextureBindingInfo> m_texturesBindingInfo;

		//Must match the parameter schema hash of the base material otherwise it needs to be resolved and
		//the parameters and textures of the base material will be used.
		Core::Sid m_parametersSchemaHash;

		//Non-reflected data

		//Cached pointer to the base material
		mutable Systems::MaterialAsset* m_pBaseMaterial;

		START_REFLECTION(Systems::MaterialInstanceAsset)
			ADD_BASETYPE(Systems::AssetObject)
			ADD_FIELD(m_material)
			ADD_FIELD(m_perMaterialParameters)
			ADD_FIELD(m_texturesBindingInfo)
			ADD_FIELD_ATTR(m_parametersSchemaHash, Core::FieldAttribute::Hidden)
		END_REFLECTION()
	};
}
