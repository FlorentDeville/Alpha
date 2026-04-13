/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Editors/ParticleEditor/ParticleListModel.h"

#include "Core/Collections/Array.h"

#include "Systems/Assets/AssetMgr.h"
#include "Systems/Assets/AssetObjects/ParticleEffect/ParticleEffectAsset.h"
#include "Systems/Assets/Metadata/AssetMetadata.h"

#include "Widgets/Models/ModelIndex.h"
#include "Widgets/Models/SelectionModel.h"
#include "Widgets/Models/SelectionRow.h"

#include <algorithm>

//#pragma optimize("", off)

namespace Editors
{
	ParticleListModel::ParticleListModel()
		: Widgets::AbstractViewModel()
		, m_cache()
	{
		Systems::AssetMgr& assetMgr = Systems::AssetMgr::Get();

		Core::Array<Core::Sid> assetTypes;
		assetTypes.PushBack(Systems::ParticleEffectAsset::GetAssetTypeNameSid());

		Core::Array<const Systems::AssetMetadata*> metadataArray;
		assetMgr.GetAssets(assetTypes, metadataArray);

		for (const Systems::AssetMetadata* pMetadata : metadataArray)
		{
			CachedParticleEffectData cachedData;
			CreateCachedData(*pMetadata, cachedData);
			m_cache.PushBack(cachedData);
		}

		SortCachedData();
	}

	ParticleListModel::~ParticleListModel()
	{ }

	Widgets::ModelIndex ParticleListModel::GetParent(const Widgets::ModelIndex& index) const
	{
		return Widgets::ModelIndex(); //no parent in a flat list
	}

	Widgets::ModelIndex ParticleListModel::GetIndex(int row, int column, const Widgets::ModelIndex& parent) const
	{
		if (parent.IsValid())
			return Widgets::ModelIndex(); //it's a flat list, only the root has children

		if (row < 0 || row >= static_cast<int>(m_cache.GetSize()))
			return Widgets::ModelIndex(); //out of bounds

		if (column < 0 || column >= Columns::Count)
			return Widgets::ModelIndex(); //out of bounds

		const CachedParticleEffectData& cachedData = m_cache[row];
		return CreateIndex(row, column, &cachedData);
	}

	int ParticleListModel::GetRowCount(const Widgets::ModelIndex& parent) const
	{
		if (parent.IsValid())
			return 0;

		return m_cache.GetSize();
	}

	int ParticleListModel::GetColumnCount(const Widgets::ModelIndex& parent) const
	{
		if (parent.IsValid())
			return 0;

		return Columns::Count;
	}

	std::string ParticleListModel::GetData(const Widgets::ModelIndex& index)
	{
		if (!index.IsValid())
			return "";

		const CachedParticleEffectData* pCachedData = static_cast<const CachedParticleEffectData*>(index.GetConstDataPointer());
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

		case Columns::Modified:
			return pCachedData->m_modified ? "*" : "";
			break;
		}

		return "__ERROR__";
	}

	std::string ParticleListModel::GetHeaderData(int column)
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
		}

		return "__ERROR__";
	}

	void ParticleListModel::AddRow(const Systems::AssetMetadata& metadata)
	{
		CachedParticleEffectData cachedData;
		CreateCachedData(metadata, cachedData);

		int row = m_cache.GetSize();
		m_cache.PushBack(cachedData);

		SortCachedData();
		Widgets::ModelIndex index = GetIndex(cachedData.m_id);
		CommitInsertRows(index.GetRow(), 1, Widgets::ModelIndex());
	}

	void ParticleListModel::RemoveRow(Systems::NewAssetId id)
	{
		Core::Array<CachedParticleEffectData>::Iterator it = std::find_if(m_cache.begin(), m_cache.end(), [id](const CachedParticleEffectData& data) { return data.m_id == id; });
		if (it == m_cache.end())
			return;

		size_t index = std::distance(m_cache.begin(), it);
		int row = static_cast<int>(index);

		BeforeRemoveRows(row, 1, Widgets::ModelIndex());

		m_cache.Erase(it);

		AfterRemoveRows(row, 1, Widgets::ModelIndex());
	}

	void ParticleListModel::SetModifiedMark(Systems::NewAssetId id)
	{
		Widgets::ModelIndex index = GetIndex(id);
		if (!index.IsValid())
			return;

		int row = index.GetRow();
		if (row < 0 || static_cast<uint32_t>(row) >= m_cache.GetSize())
			return;

		m_cache[row].m_modified = true;

		Widgets::ModelIndex modifiedIndex = index.GetSiblingAtColumn(Columns::Modified);
		m_onDataChanged(modifiedIndex);
	}

	void ParticleListModel::ClearModifiedMark(Systems::NewAssetId id)
	{
		Widgets::ModelIndex index = GetIndex(id);
		if (!index.IsValid())
			return;

		int row = index.GetRow();
		if (row < 0 || static_cast<uint32_t>(row) >= m_cache.GetSize())
			return;

		m_cache[row].m_modified = false;

		Widgets::ModelIndex modifiedIndex = index.GetSiblingAtColumn(Columns::Modified);
		m_onDataChanged(modifiedIndex);
	}

	void ParticleListModel::OnParticleEffectRenamed(const Systems::AssetMetadata& metadata)
	{
		Widgets::ModelIndex index = GetIndex(metadata.GetAssetId());
		if (!index.IsValid())
			return;

		CachedParticleEffectData& data = m_cache[index.GetRow()];
		data.m_virtualName = metadata.GetVirtualName();

		m_onDataChanged(index.GetSiblingAtColumn(Columns::Name));
	}

	Systems::NewAssetId ParticleListModel::GetAssetId(const Widgets::ModelIndex& index) const
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

	void ParticleListModel::SetSelection(Systems::NewAssetId id)
	{
		Widgets::ModelIndex index = GetIndex(id);
		if (!index.IsValid())
			return;

		Widgets::SelectionModel* pSelectionModel = GetSelectionModel();

		Widgets::SelectionRow row(index, index);
		pSelectionModel->SetSelectionRow(row);
	}

	Systems::NewAssetId ParticleListModel::GetSelection() const
	{
		const Widgets::SelectionModel* pSelectionModel = GetSelectionModel();
		const std::list<Widgets::SelectionRow>& selectedRows = pSelectionModel->GetSelectedRows();
		if (selectedRows.empty())
			return Systems::NewAssetId::INVALID;

		Widgets::ModelIndex index = selectedRows.back().GetStartIndex();
		return GetAssetId(index);
	}

	Widgets::ModelIndex ParticleListModel::GetIndex(Systems::NewAssetId id) const
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

	void ParticleListModel::CreateCachedData(const Systems::AssetMetadata& metadata, CachedParticleEffectData& cache) const
	{
		cache.m_id = metadata.GetAssetId();
		cache.m_virtualName = metadata.GetVirtualName();
		cache.m_modified = false;
	}

	void ParticleListModel::SortCachedData()
	{
		std::sort(m_cache.begin(), m_cache.end(), [](const CachedParticleEffectData& data1, const CachedParticleEffectData& data2) { return data1.m_virtualName < data2.m_virtualName; });
	}
}