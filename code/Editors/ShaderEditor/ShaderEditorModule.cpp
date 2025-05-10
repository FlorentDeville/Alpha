/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/ShaderEditor/ShaderEditorModule.h"

#include "Systems/Assets/AssetMgr.h"
#include "Systems/Assets/AssetObjects/Material/MaterialAsset.h"
#include "Systems/Container/Container.h"
#include "Systems/Container/ContainerMgr.h"

namespace Editors
{
	void ShaderEditorModule::Init()
	{
		const Systems::AssetMgr& assetMgr = Systems::AssetMgr::Get();

		assetMgr.ForEachMetadata([this](const Systems::AssetMetadata& metadata)
			{
				if (metadata.GetAssetType() != MAKESID("Material"))
					return;

				m_allShaders.push_back(metadata.GetAssetId());
			});
	}

	bool ShaderEditorModule::NewShader(const std::string& virtualName)
	{
		Systems::MaterialAsset* pNewMaterial = Systems::CreateNewAsset<Systems::MaterialAsset>();

		Systems::ContainerMgr& containerMgr = Systems::ContainerMgr::Get();
		Systems::Container* pContainer = containerMgr.CreateContainer(virtualName.c_str());
		pContainer->AddAsset(pNewMaterial);
		bool res = containerMgr.SaveContainer(pContainer->GetId());
		if (!res)
			return false;

		Systems::AssetMetadata materialMetadata(pNewMaterial->GetId(), virtualName, MAKESID("Material"));
		Systems::AssetMgr& assetMgr = Systems::AssetMgr::Get();
		res = assetMgr.RegisterAssetMetadata(materialMetadata);
		if (!res)
			return false;

		res = assetMgr.SaveMetadataTable();
		if (!res)
			return false;

		m_allShaders.push_back(pNewMaterial->GetId());

		return true;
	}

	bool ShaderEditorModule::SaveShader(Systems::NewAssetId id)
	{
		Systems::ContainerMgr& containerMgr = Systems::ContainerMgr::Get();
		bool res = containerMgr.SaveContainer(id.GetContainerId());
		return res;
	}

	const std::vector<Systems::NewAssetId>& ShaderEditorModule::GetAllShaders() const
	{
		return m_allShaders;
	}
}
