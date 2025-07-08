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
	{ }

	MaterialInstanceAsset_v1::~MaterialInstanceAsset_v1()
	{ }

	bool MaterialInstanceAsset_v1::InitialiseFromBaseMaterial(Systems::MaterialAsset* pBaseMaterial)
	{
		m_pBaseMaterial = pBaseMaterial;
		m_material = pBaseMaterial->GetId();

		const Core::Array<MaterialParameterDescription>& parameters = pBaseMaterial->GetMaterialParameterDescription();
		m_perMaterialParameters = parameters;

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
