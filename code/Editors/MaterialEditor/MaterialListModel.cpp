/********************************************************************************/
/* Copyright (C) 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/


#include "Editors/MaterialEditor/MaterialListModel.h"

#include "Editors/MaterialEditor/MaterialEditorModule.h"

#include "Systems/Assets/AssetMgr.h"
#include "Systems/Assets/AssetObjects/AssetUtil.h"
#include "Systems/Assets/AssetObjects/MaterialInstance/MaterialInstanceAsset.h"

#include "Widgets/Models/ModelIndex.h"

namespace Editors
{
	MaterialListModel::MaterialListModel()
		: AbstractViewModel()
	{
		const Systems::AssetMgr& assetMgr = Systems::AssetMgr::Get();
		const MaterialEditorModule& module = MaterialEditorModule::Get();
		const std::vector<Systems::NewAssetId>& allShaders = module.GetAllMaterials();
		for (Systems::NewAssetId id : allShaders)
		{
			const Systems::AssetMetadata* pMetadata = assetMgr.GetMetadata(id);
			AddToCache(pMetadata);
		}
	}

	MaterialListModel::~MaterialListModel()
	{ }

	Widgets::ModelIndex MaterialListModel::GetParent(const Widgets::ModelIndex&) const
	{
		return Widgets::ModelIndex(); //there is no parent in arrays
	}

	Widgets::ModelIndex MaterialListModel::GetIndex(int row, int column, const Widgets::ModelIndex& parent) const
	{
		if (parent.IsValid())
			return Widgets::ModelIndex();

		if (row < 0 || row >= m_cache.size())
			return Widgets::ModelIndex();

		if (column < 0 || column >= Columns::Count)
			return Widgets::ModelIndex();

		const CachedMaterialData& data = m_cache[row];
		return CreateIndex(row, column, &data);
	}

	int MaterialListModel::GetRowCount(const Widgets::ModelIndex& parent) const
	{
		if (parent.IsValid())
			return 0;

		return static_cast<int>(m_cache.size());
	}

	int MaterialListModel::GetColumnCount(const Widgets::ModelIndex& parent) const
	{
		if (parent.IsValid())
			return 0;

		return Columns::Count;
	}

	std::string MaterialListModel::GetData(const Widgets::ModelIndex& index)
	{
		const CachedMaterialData* pData = reinterpret_cast<const CachedMaterialData*>(index.GetConstDataPointer());
		switch (index.GetColumn())
		{
		case Columns::Id:
			return pData->m_id.ToString();
			break;

		case Columns::Name:
			return pData->m_virtualName;
			break;

		case Columns::Type:
		{
			switch (pData->m_type)
			{
			case CachedMaterialData::Unknown:
				return "Unknown";
				break;

			case CachedMaterialData::Material:
				return "Material";
				break;

			case CachedMaterialData::MaterialInstance:
				return "Material Instance";
				break;
			}
		}
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
		return "__ERROR__";
	}

	std::string MaterialListModel::GetHeaderData(int column)
	{
		switch (column)
		{
		case Columns::Id:
			return "Id";
			break;

		case Columns::Name:
			return "Name";
			break;

		case Columns::Type:
			return "Type";
			break;

		case Columns::Modified:
			return "Modified";
			break;

		default:
			return "";
			break;
		}
	}

	void MaterialListModel::AddRow(const Systems::AssetMetadata* pMetadata)
	{
		int row = static_cast<int>(m_cache.size());
		
		AddToCache(pMetadata);
		
		CommitInsertRows(row, 1, Widgets::ModelIndex());
	}

	void MaterialListModel::RemoveRow(Systems::NewAssetId id)
	{
		int cacheIndex = FindCacheIndex(id);
		if (cacheIndex < 0)
			return;

		BeforeRemoveRows(cacheIndex, 1, Widgets::ModelIndex());

		std::vector<CachedMaterialData>::const_iterator it = m_cache.cbegin() + cacheIndex;
		m_cache.erase(it);

		AfterRemoveRows(cacheIndex, 1, Widgets::ModelIndex());
	}

	void MaterialListModel::SetShaderModified(Systems::NewAssetId id)
	{
		int cacheIndex = FindCacheIndex(id);
		if (cacheIndex == -1)
			return;

		if (m_cache[cacheIndex].m_modified)
			return;

		m_cache[cacheIndex].m_modified = true;
		m_onDataChanged(GetIndex(cacheIndex, Columns::Modified, Widgets::ModelIndex()));
	}

	void MaterialListModel::ClearShaderModified(Systems::NewAssetId id)
	{
		int cacheIndex = FindCacheIndex(id);
		if (cacheIndex == -1)
			return;

		if (!m_cache[cacheIndex].m_modified)
			return;

		m_cache[cacheIndex].m_modified = false;
		m_onDataChanged(GetIndex(cacheIndex, Columns::Modified, Widgets::ModelIndex()));
	}

	void MaterialListModel::OnMaterialRenamed(const Systems::AssetMetadata& metadata)
	{
		//first update the cache
		int cacheIndex = FindCacheIndex(metadata.GetAssetId());
		if (cacheIndex == -1)
			return;

		m_cache[cacheIndex].m_virtualName = metadata.GetVirtualName();

		Widgets::ModelIndex index = GetIndex(metadata.GetAssetId());
		index = index.GetSiblingAtColumn(Columns::Name);
		m_onDataChanged(index);
	}

	Systems::NewAssetId MaterialListModel::GetAssetId(const Widgets::ModelIndex& index) const
	{
		const CachedMaterialData* pData = reinterpret_cast<const CachedMaterialData*>(index.GetConstDataPointer());
		return pData->m_id;
	}

	Widgets::ModelIndex MaterialListModel::GetIndex(Systems::NewAssetId id) const
	{
		int index = FindCacheIndex(id);
		if(index == -1)
			return Widgets::ModelIndex();
		
		return GetIndex(index, 0, Widgets::ModelIndex());
	}

	void MaterialListModel::AddToCache(const Systems::AssetMetadata* pMetadata)
	{
		m_cache.push_back(CachedMaterialData());
		CachedMaterialData& data = m_cache.back();
		data.m_id = pMetadata->GetAssetId();
		data.m_virtualName = pMetadata->GetVirtualName();
		data.m_modified = false;
		if (pMetadata->IsA<Systems::MaterialAsset>())
			data.m_type = CachedMaterialData::Material;
		else if (pMetadata->IsA<Systems::MaterialInstanceAsset>())
			data.m_type = CachedMaterialData::MaterialInstance;
		else
			data.m_type = CachedMaterialData::Unknown;
	}

	int MaterialListModel::FindCacheIndex(Systems::NewAssetId id) const
	{
		for (size_t ii = 0; ii < m_cache.size(); ++ii)
		{
			const CachedMaterialData& data = m_cache[ii];
			if (data.m_id == id)
				return static_cast<int>(ii);
		}
		
		return -1;
	}
}
