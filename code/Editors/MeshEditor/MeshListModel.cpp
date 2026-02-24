/********************************************************************/
/* © 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/MeshEditor/MeshListModel.h"

#include "Core/Collections/Array.h"

#include "Systems/Assets/AssetMgr.h"
#include "Systems/Assets/AssetObjects/Mesh/MeshAsset.h"
#include "Systems/Assets/Metadata/AssetMetadata.h"

#include "Widgets/Models/ModelIndex.h"

//#pragma optimize("", off)

namespace Editors
{
	MeshListModel::MeshListModel()
		: Widgets::AbstractViewModel()
		, m_cache()
	{
		Systems::AssetMgr& assetMgr = Systems::AssetMgr::Get();

		Core::Array<const Systems::AssetMetadata*> metadataArray;
		assetMgr.GetAssets(Systems::MeshAsset::GetAssetTypeNameSid(), metadataArray);

		for (const Systems::AssetMetadata* pMetadata : metadataArray)
		{
			CachedMeshData cachedData;
			cachedData.m_id = pMetadata->GetAssetId();
			cachedData.m_virtualName = pMetadata->GetVirtualName();

			m_cache.PushBack(cachedData);
		}
	}

	MeshListModel::~MeshListModel()
	{
	}

	Widgets::ModelIndex MeshListModel::GetParent(const Widgets::ModelIndex& index) const
	{
		return Widgets::ModelIndex(); //no parent in a flat list
	}

	Widgets::ModelIndex MeshListModel::GetIndex(int row, int column, const Widgets::ModelIndex& parent) const
	{
		if (parent.IsValid())
			return Widgets::ModelIndex(); //it's a flat list, only the root has children

		if (row < 0 || row >= static_cast<int>(m_cache.GetSize()))
			return Widgets::ModelIndex(); //out of bounds

		if (column < 0 || column >= Columns::Count)
			return Widgets::ModelIndex(); //out of bounds

		const CachedMeshData& cachedData = m_cache[row];
		return CreateIndex(row, column, &cachedData);
	}

	int MeshListModel::GetRowCount(const Widgets::ModelIndex& parent) const
	{
		if (parent.IsValid())
			return 0;

		return m_cache.GetSize();
	}

	int MeshListModel::GetColumnCount(const Widgets::ModelIndex& parent) const
	{
		if (parent.IsValid())
			return 0;

		return Columns::Count;
	}

	std::string MeshListModel::GetData(const Widgets::ModelIndex& index)
	{
		if (!index.IsValid())
			return "";

		const CachedMeshData* pCachedData = static_cast<const CachedMeshData*>(index.GetConstDataPointer());
		if (!pCachedData)
			return "";

		switch (index.GetColumn())
		{
		case Columns::Id:
			return pCachedData->m_id.ToString();
			break;

		case Columns::Name:
			return pCachedData->m_virtualName;
			break;

		case Columns::ReImport:
			return "reimport";
			break;
		}

		return "__ERROR__";
	}

	std::string MeshListModel::GetHeaderData(int column)
	{
		switch (column)
		{
		case Columns::Id:
			return "Id";
			break;

		case Columns::Name:
			return "Name";
			break;

		case Columns::ReImport:
			return "ReImport";
			break;
		}

		return "__ERROR__";
	}

	void MeshListModel::AddRow(const Systems::AssetMetadata& metadata)
	{
		if (!metadata.IsA<Systems::MeshAsset>())
			return;

		CachedMeshData cachedData;
		cachedData.m_id = metadata.GetAssetId();
		cachedData.m_virtualName = metadata.GetVirtualName();

		int row = m_cache.GetSize();
		m_cache.PushBack(cachedData);

		CommitInsertRows(row, 1, Widgets::ModelIndex());
	}

	void MeshListModel::RemoveRow(Systems::NewAssetId id)
	{
		Core::Array<CachedMeshData>::Iterator it = std::find_if(m_cache.begin(), m_cache.end(), [id](const CachedMeshData& data) { return data.m_id == id; });
		if (it == m_cache.end())
			return;

		size_t index = std::distance(m_cache.begin(), it);
		int row = static_cast<int>(index);

		BeforeRemoveRows(row, 1, Widgets::ModelIndex());

		m_cache.Erase(it);

		AfterRemoveRows(row, 1, Widgets::ModelIndex());
	}

	void MeshListModel::OnMeshRenamed(const Systems::AssetMetadata& metadata)
	{
		Widgets::ModelIndex index = GetIndex(metadata.GetAssetId());
		if (!index.IsValid())
			return;

		CachedMeshData& data = m_cache[index.GetRow()];
		data.m_virtualName = metadata.GetVirtualName();

		m_onDataChanged(index.GetSiblingAtColumn(Columns::Name));
	}

	Systems::NewAssetId MeshListModel::GetAssetId(const Widgets::ModelIndex& index) const
	{
		if (!index.IsValid())
			return Systems::NewAssetId::INVALID;

		if (index.GetParent().IsValid())
			return Systems::NewAssetId::INVALID;

		uint32_t row = static_cast<uint32_t>(index.GetRow());
		if (row >= m_cache.GetSize())
			return Systems::NewAssetId::INVALID;

		return m_cache[row].m_id;
	}

	Widgets::ModelIndex MeshListModel::GetIndex(Systems::NewAssetId id) const
	{
		int row = -1;
		for (uint32_t ii = 0; ii < m_cache.GetSize(); ++ii)
		{
			if (m_cache[ii].m_id == id)
			{
				row = static_cast<int>(ii);
				break;
			}
		}

		if (row == -1)
			return Widgets::ModelIndex();

		return CreateIndex(row, 0, &m_cache[row]);
	}
}