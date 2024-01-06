/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/Widgets/List/Models/AssetListModel.h"

#include "Systems/Assets/Asset.h"
#include "Systems/Assets/AssetMgr.h"

#include "Widgets/Models/ModelIndex.h"

#include <cassert>

namespace Editors
{
	std::string AssetListModel::s_invalidValue = "DEADMEAT";

	AssetListModel::AssetListModel(Systems::AssetType type)
		: AbstractViewModel()
		, m_assetType(type)
	{
		const Systems::AssetMgr& assetMgr = Systems::AssetMgr::Get();
		const std::vector<Systems::Asset*>& assetList = assetMgr.GetAssets(m_assetType);
	
		for (const Systems::Asset* pAsset : assetList)
		{
			m_cachedAssetId.push_back(pAsset->GetId().ToString());
		}
	}

	AssetListModel::~AssetListModel()
	{}

	Widgets::ModelIndex AssetListModel::GetParent(const Widgets::ModelIndex& child) const
	{
		return Widgets::ModelIndex(); //in an array, the parent is always root
	}

	Widgets::ModelIndex AssetListModel::GetIndex(int row, int column, const Widgets::ModelIndex& parent) const
	{
		//parent must be the invalid index. This is an array, rows don't have children
		if (parent.IsValid())
			return Widgets::ModelIndex();

		//this array only had 2 columns
		if (column > 2)
			return Widgets::ModelIndex();

		//get the data pointer
		const Systems::AssetMgr& assetMgr = Systems::AssetMgr::Get();
		const std::vector<Systems::Asset*>& assetList = assetMgr.GetAssets(m_assetType);
		
		//we should have an internal cached copy inside the model
		Systems::Asset* pAsset = assetList[row];
		return CreateIndex(row, column, pAsset);
	}

	int AssetListModel::GetRowCount(const Widgets::ModelIndex& parent)
	{
		//only root has children
		if (parent.IsValid())
			return 0;

		//only root in the first column has children
		if (parent.GetColumn() > 0)
			return 0;

		const Systems::AssetMgr& assetMgr = Systems::AssetMgr::Get();
		const std::vector<Systems::Asset*>& assetList = assetMgr.GetAssets(m_assetType);
		return static_cast<int>(assetList.size());
	}

	int AssetListModel::GetColumnCount(const Widgets::ModelIndex& parent) const
	{
		if (parent.IsValid())
			return 0;

		if (parent.GetColumn() > 0)
			return 0;

		//column 0 : id
		//column 1 : virtual path
		return 2;
	}

	std::string AssetListModel::GetData(const Widgets::ModelIndex& idx)
	{
		const void* pData = idx.GetConstDataPointer();
		const Systems::Asset* pAsset = reinterpret_cast<const Systems::Asset*>(pData);

		if (idx.GetColumn() == 0)
		{
			return pAsset->GetId().ToString();
		}
		else if (idx.GetColumn() == 1)
		{
			return pAsset->GetVirtualName();
		}

		return "";
	}

	void AssetListModel::InsertRows(int start, int count)
	{
		CommitInsertRows(start, count, Widgets::ModelIndex());
	}
}
