/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Systems/Assets/AssetObjects/MaterialInstance/MaterialInstanceAsset_v1.h"

namespace Systems
{
	MaterialInstanceAsset_v1::MaterialInstanceAsset_v1()
		: AssetObject()
	{ }

	MaterialInstanceAsset_v1::~MaterialInstanceAsset_v1()
	{ }

	bool MaterialInstanceAsset_v1::InitialiseFromBaseMaterial(Systems::MaterialAsset& baseMaterial)
	{
		m_material = baseMaterial.GetId();

		const Core::Array<MaterialParameterDescription>& parameters = baseMaterial.GetMaterialParameterDescription();
		m_perMaterialParameters = parameters;

		return true;
	}

	NewAssetId MaterialInstanceAsset_v1::GetBaseMaterialId() const
	{
		return m_material;
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
