/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/ShaderEditor/ShaderEditorModule.h"

#include "Systems/Assets/AssetMgr.h"
#include "Systems/Assets/AssetObjects/AssetUtil.h"
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

	Systems::MaterialAsset* ShaderEditorModule::NewShader(const std::string& virtualName)
	{
		Systems::MaterialAsset* pNewMaterial = Systems::CreateNewAsset<Systems::MaterialAsset>();
		assert(pNewMaterial);

		Systems::ContainerMgr& containerMgr = Systems::ContainerMgr::Get();
		Systems::Container* pContainer = containerMgr.CreateContainer(virtualName.c_str());
		pContainer->AddAsset(pNewMaterial);
		bool res = containerMgr.SaveContainer(pContainer->GetId());
		if (!res)
			return nullptr;

		Systems::AssetMetadata materialMetadata(pNewMaterial->GetId(), virtualName, MAKESID("Material"));
		Systems::AssetMgr& assetMgr = Systems::AssetMgr::Get();
		res = assetMgr.RegisterAssetMetadata(materialMetadata);
		if (!res)
			return nullptr;

		res = assetMgr.SaveMetadataTable();
		if (!res)
			return nullptr;

		m_allShaders.push_back(pNewMaterial->GetId());

		m_onShaderCreated(&materialMetadata);

		return pNewMaterial;
	}

	bool ShaderEditorModule::SaveShader(Systems::NewAssetId id)
	{
		Systems::ContainerMgr& containerMgr = Systems::ContainerMgr::Get();
		bool res = containerMgr.SaveContainer(id.GetContainerId());
		return res;
	}

	bool ShaderEditorModule::DeleteShader(Systems::NewAssetId id)
	{
		bool res = Systems::AssetUtil::DeleteAsset(id);
		if (!res)
			return false;

		std::vector<Systems::NewAssetId>::const_iterator it = std::find(m_allShaders.cbegin(), m_allShaders.cend(), id);
		if (it == m_allShaders.cend())
			return true;

		m_allShaders.erase(it);
		return true;
	}

	const std::vector<Systems::NewAssetId>& ShaderEditorModule::GetAllShaders() const
	{
		return m_allShaders;
	}
}
