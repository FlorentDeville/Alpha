/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/LevelEditor/LevelListModel.h"

#include "Systems/Assets/AssetMgr.h"
#include "Systems/Assets/AssetObjects/Level/LevelAsset.h"

#include "Widgets/Models/ModelIndex.h"

namespace Editors
{
	LevelListModel::LevelListModel()
		: AbstractViewModel()
	{
		const Systems::AssetMgr& assetMgr = Systems::AssetMgr::Get();
		assetMgr.ForEachMetadata([this](const Systems::AssetMetadata& metadata) 
			{
				if (!metadata.IsA<Systems::LevelAsset>())
					return;
				
				AddCachedData(metadata);
			});
	}

	LevelListModel::~LevelListModel()
	{
	}

	Widgets::ModelIndex LevelListModel::GetParent(const Widgets::ModelIndex&) const
	{
		return Widgets::ModelIndex(); //there is no parent in arrays
	}

	Widgets::ModelIndex LevelListModel::GetIndex(int row, int column, const Widgets::ModelIndex& parent) const
	{
		if (parent.IsValid())
			return Widgets::ModelIndex();

		if (row < 0 || row >= static_cast<int32_t>(m_cachedDataArray.GetSize()))
			return Widgets::ModelIndex();

		if (column < 0 || column >= Columns::Count)
			return Widgets::ModelIndex();

		const CachedLevelData& data = m_cachedDataArray[row];
		return CreateIndex(row, column, &data);
	}

	int LevelListModel::GetRowCount(const Widgets::ModelIndex& parent) const
	{
		if (parent.IsValid())
			return 0;

		return static_cast<int>(m_cachedDataArray.GetSize());
	}

	int LevelListModel::GetColumnCount(const Widgets::ModelIndex& parent) const
	{
		if (parent.IsValid())
			return 0;

		return Columns::Count;
	}

	std::string LevelListModel::GetData(const Widgets::ModelIndex& index)
	{
		const CachedLevelData* pData = reinterpret_cast<const CachedLevelData*>(index.GetConstDataPointer());
		switch (index.GetColumn())
		{
		case Columns::Id:
			return pData->m_id.ToString();
			break;

		case Columns::Name:
			return pData->m_virtualName;
			break;

		case Columns::Modified:
		{
			if (pData->m_modified)
				return "*";
			else
				return "";
		}
		break;

		default:
			return "";
			break;
		}
	}

	std::string LevelListModel::GetHeaderData(int column)
	{
		switch (column)
		{
		case Columns::Id:
			return "Id";
			break;

		case Columns::Name:
			return "Name";
			break;

		case Columns::Modified:
			return "Modified";
			break;

		default:
			return "";
			break;
		}
	}

	void LevelListModel::AddNewLevel(const Systems::AssetMetadata& metadata)
	{
		uint32_t row = m_cachedDataArray.GetSize();

		AddCachedData(metadata);

		CommitInsertRows(row, 1, Widgets::ModelIndex());
	}

	Systems::NewAssetId LevelListModel::GetAssetId(const Widgets::ModelIndex& index) const
	{
		if (index.GetParent().IsValid())
			return Systems::NewAssetId::INVALID;

		int row = index.GetRow();
		if (row < 0 || row >= static_cast<int>(m_cachedDataArray.GetSize()))
			return Systems::NewAssetId::INVALID;

		const CachedLevelData& data = m_cachedDataArray[row];
		return data.m_id;
	}

	void LevelListModel::RenameLevel(Systems::NewAssetId id, const std::string& newName)
	{
		Core::Array<CachedLevelData>::Iterator it = std::find_if(m_cachedDataArray.begin(), m_cachedDataArray.end(), [&id](const CachedLevelData& data) { return data.m_id == id; });
		if (it == m_cachedDataArray.end())
			return;

		it->m_virtualName = newName;

		size_t row = std::distance(m_cachedDataArray.begin(), it);
		m_onDataChanged(GetIndex(static_cast<int>(row), Columns::Name, Widgets::ModelIndex()));
	}

	void LevelListModel::RemoveLevel(const Systems::AssetMetadata& metadata)
	{
		Core::Array<CachedLevelData>::Iterator it = std::find_if(m_cachedDataArray.begin(), m_cachedDataArray.end(), [&metadata](const CachedLevelData& data) { return data.m_id == metadata.GetAssetId(); });
		if (it == m_cachedDataArray.end())
			return;
		
		size_t index = std::distance(m_cachedDataArray.begin(), it);

		m_cachedDataArray.Erase(it);

		RemoveRows(static_cast<int>(index), 1, Widgets::ModelIndex());
	}

	void LevelListModel::AddCachedData(const Systems::AssetMetadata& metadata)
	{
		CachedLevelData data;
		data.m_id = metadata.GetAssetId();
		data.m_virtualName = metadata.GetVirtualName();
		data.m_modified = false;
		m_cachedDataArray.PushBack(data);
	}
}
