/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Systems/Assets/AssetObjects/MaterialInstance/MaterialInstanceAsset_v1.h"

#include "Systems/Assets/AssetObjects/AssetUtil.h"

namespace Systems
{
	MaterialInstanceAsset_v1::MaterialInstanceAsset_v1()
		: AssetObject()
		, m_pBaseMaterial(nullptr)
		, m_perMaterialParameters()
		, m_texturesBindingInfo()
	{ }

	MaterialInstanceAsset_v1::~MaterialInstanceAsset_v1()
	{ }

	bool MaterialInstanceAsset_v1::InitialiseFromBaseMaterial(Systems::MaterialAsset* pBaseMaterial)
	{
		if (pBaseMaterial)
		{
			m_pBaseMaterial = pBaseMaterial;
			m_material = pBaseMaterial->GetId();

			const Core::Array<MaterialParameterDescription>& parameters = pBaseMaterial->GetMaterialParameterDescription();
			m_perMaterialParameters = parameters;

			const Core::Array<TextureBindingInfo>& textures = pBaseMaterial->GetTexturesBindingInfo();
			m_texturesBindingInfo = textures;
		}
		else
		{
			m_pBaseMaterial = nullptr;
			m_material = Systems::NewAssetId::INVALID;
			m_perMaterialParameters.Clear();
			m_texturesBindingInfo.Clear();
		}
		return true;
	}

	bool MaterialInstanceAsset_v1::Refresh()
	{
		if (!m_pBaseMaterial)
			return false;

		//coppy parameters
		const Core::Array<MaterialParameterDescription>& parameters = m_pBaseMaterial->GetMaterialParameterDescription();
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
		const Core::Array<TextureBindingInfo>& textures = m_pBaseMaterial->GetTexturesBindingInfo();
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

		return true;
	}

	NewAssetId MaterialInstanceAsset_v1::GetBaseMaterialId() const
	{
		return m_material;
	}

	const Systems::MaterialAsset* MaterialInstanceAsset_v1::GetBaseMaterial() const
	{
		if (!m_pBaseMaterial && m_material.IsValid())
			m_pBaseMaterial = Systems::AssetUtil::LoadAsset<Systems::MaterialAsset>(m_material);

		return m_pBaseMaterial;
	}

	const Core::Array<MaterialParameterDescription>& MaterialInstanceAsset_v1::GetMaterialParameterDescription() const
	{
		return m_perMaterialParameters;
	}

	const Core::Array<TextureBindingInfo>& MaterialInstanceAsset_v1::GetTexturesBindingInfo() const
	{
		return m_texturesBindingInfo;
	}

	const std::string& MaterialInstanceAsset_v1::GetAssetTypeName()
	{
		static std::string name = "MaterialInstance";
		return name;
	}

	Core::Sid MaterialInstanceAsset_v1::GetAssetTypeNameSid()
	{
		static Core::Sid sid = SID(GetAssetTypeName());
		return sid;
	}

}
