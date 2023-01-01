/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/Widgets/List/Models/AssetListModel.h"

#include "Systems/Assets/Asset.h"
#include "Systems/Assets/AssetMgr.h"

namespace Editors
{
	std::string AssetListModel::s_invalidValue = "DEADMEAT";

	AssetListModel::AssetListModel(Systems::AssetType type)
		: BaseModel()
		, m_assetType(type)
	{
		const std::vector<Systems::Asset*>* pAssetList = nullptr;

		const Systems::AssetMgr& assetMgr = Systems::AssetMgr::Get();
		switch (m_assetType)
		{
		case Systems::kMaterial:
			pAssetList = &assetMgr.GetMaterials();
			break;

		case Systems::kMesh:
			pAssetList = &assetMgr.GetMeshes();
			break;

		default:
			assert(false);
		}

		for (const Systems::Asset* pAsset : *pAssetList)
		{
			m_cachedAssetId.push_back(pAsset->GetId().ToString());
		}
	}

	AssetListModel::~AssetListModel()
	{}

	int AssetListModel::GetRowCount() const
	{
		const Systems::AssetMgr& assetMgr = Systems::AssetMgr::Get();
		switch (m_assetType)
		{
		case Systems::kMaterial:
			return static_cast<int>(assetMgr.GetMaterials().size());
			break;

		case Systems::kMesh:
			return static_cast<int>(assetMgr.GetMeshes().size());
			break;

		default:
			assert(false);
		}

		return 0;
	}

	int AssetListModel::GetColumnCount() const
	{
		//0 : id
		//1 : virtual path
		return 2;
	}

	const std::string& AssetListModel::GetData(int rowId, int columnId) const
	{
		const std::vector<Systems::Asset*>* pAssetList = nullptr;

		const Systems::AssetMgr& assetMgr = Systems::AssetMgr::Get();
		switch (m_assetType)
		{
		case Systems::kMaterial:
			pAssetList = &assetMgr.GetMaterials();
			break;

		case Systems::kMesh:
			pAssetList = &assetMgr.GetMeshes();
			break;

		default:
			assert(false);
		}

		Systems::Asset* pAsset = (*pAssetList)[rowId];

		if (columnId == 0)
		{
			return m_cachedAssetId[rowId];
		}
		else if (columnId == 1)
			return pAsset->GetVirtualName();

		assert(false);
		return s_invalidValue;
	}
}
