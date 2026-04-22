/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Systems/Assets/AssetObjects/MaterialInstance/MaterialInstanceAsset.h"

#include "Systems/Assets/AssetObjects/AssetUtil.h"

namespace Systems
{
	MaterialInstanceAsset::MaterialInstanceAsset()
		: AssetObject()
		, m_baseMaterial()
		, m_perMaterialParameters()
		, m_texturesBindingInfo()
		, m_parametersSchemaHash()
	{ }

	MaterialInstanceAsset::~MaterialInstanceAsset()
	{ }

	bool MaterialInstanceAsset::InitialiseFromBaseMaterial(Systems::MaterialAsset* pBaseMaterial)
	{
		if (pBaseMaterial)
		{
			m_baseMaterial = HardAssetRef<MaterialAsset>(pBaseMaterial->GetId());
			m_baseMaterial.Load(LoadingDomain::EDITOR);

			const Core::Array<MaterialParameterDescription>& parameters = pBaseMaterial->GetMaterialParameterDescription();
			m_perMaterialParameters = parameters;

			const Core::Array<TextureBindingInfo>& textures = pBaseMaterial->GetTexturesBindingInfo();
			m_texturesBindingInfo = textures;
		}
		else
		{
			m_baseMaterial = HardAssetRef<MaterialAsset>();
			m_perMaterialParameters.Clear();
			m_texturesBindingInfo.Clear();
		}
		return true;
	}

	MaterialInstanceAsset::RefreshResult MaterialInstanceAsset::Refresh()
	{
		const Systems::MaterialAsset* pBaseMaterial = GetBaseMaterial();
		if (!pBaseMaterial)
			return RefreshResult::Failure;

		if (pBaseMaterial->GetParametersSchemaHash() == m_parametersSchemaHash)
			return RefreshResult::UpToDate;

		//copy parameters
		const Core::Array<MaterialParameterDescription>& parameters = pBaseMaterial->GetMaterialParameterDescription();
		Core::Array<MaterialParameterDescription> newParameters;
		newParameters.Reserve(parameters.GetSize());
		for (const MaterialParameterDescription& param : parameters)
		{
			uint32_t index = newParameters.GetSize();
			newParameters.PushBack(param);

			//try to keep any existing value
			Core::Array<MaterialParameterDescription>::Iterator it = std::find_if(m_perMaterialParameters.cbegin(), m_perMaterialParameters.cend(), [&param](const MaterialParameterDescription& item)
				{
					return item.m_name == param.m_name && item.m_size == param.m_size && item.m_type == param.m_type;
				});

			if (it != m_perMaterialParameters.cend())
				newParameters[index].m_value = it->m_value;
		}

		m_perMaterialParameters = std::move(newParameters);

		//copy textures
		const Core::Array<TextureBindingInfo>& textures = pBaseMaterial->GetTexturesBindingInfo();
		Core::Array<TextureBindingInfo> newTextures;
		newTextures.Reserve(textures.GetSize());
		for (const TextureBindingInfo& texture : textures)
		{
			uint32_t index = newTextures.GetSize();
			newTextures.PushBack(texture);

			//try to keep any existing value
			Core::Array<TextureBindingInfo>::Iterator it = std::find_if(m_texturesBindingInfo.cbegin(), m_texturesBindingInfo.cend(), [&texture](const TextureBindingInfo& item)
				{
					return texture.m_name == item.m_name;
				});

			if (it != m_texturesBindingInfo.cend())
				newTextures[index].m_texture = it->m_texture;
		}

		m_texturesBindingInfo = std::move(newTextures);

		CalculateParametersSchemaHash();

		return RefreshResult::Success;
	}

	NewAssetId MaterialInstanceAsset::GetBaseMaterialId() const
	{
		return m_baseMaterial.GetAssetId();
	}

	const Systems::MaterialAsset* MaterialInstanceAsset::GetBaseMaterial() const
	{
		return m_baseMaterial.GetPtr();
	}

	const Core::Array<MaterialParameterDescription>& MaterialInstanceAsset::GetMaterialParameterDescription() const
	{
		return m_perMaterialParameters;
	}

	const Core::Array<TextureBindingInfo>& MaterialInstanceAsset::GetTexturesBindingInfo() const
	{
		return m_texturesBindingInfo;
	}

	Core::Sid MaterialInstanceAsset::GetParametersSchemaHash() const
	{
		return m_parametersSchemaHash;
	}

	void MaterialInstanceAsset::CalculateParametersSchemaHash()
	{
		m_parametersSchemaHash = MaterialAsset::CalculateParametersSchemaHash(m_perMaterialParameters, m_texturesBindingInfo);
	}

	const std::string& MaterialInstanceAsset::GetAssetTypeName()
	{
		static std::string name = "MaterialInstance";
		return name;
	}

	Core::Sid MaterialInstanceAsset::GetAssetTypeNameSid()
	{
		static Core::Sid sid = SID(GetAssetTypeName());
		return sid;
	}
}
