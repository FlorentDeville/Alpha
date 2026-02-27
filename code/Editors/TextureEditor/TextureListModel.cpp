/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Editors/TextureEditor/TextureListModel.h"

#include "Core/Collections/Array.h"

#include "Systems/Assets/AssetMgr.h"
#include "Systems/Assets/AssetObjects/Cubemap/Cubemap.h"
#include "Systems/Assets/AssetObjects/Texture/TextureAsset.h"
#include "Systems/Assets/Metadata/AssetMetadata.h"

#include "Widgets/Models/ModelIndex.h"

//#pragma optimize("", off)

namespace Editors
{
	TextureListModel::TextureListModel()
		: Widgets::AbstractViewModel()
		, m_cache()
	{
		Systems::AssetMgr& assetMgr = Systems::AssetMgr::Get();

		Core::Array<Core::Sid> assetTypes;
		assetTypes.PushBack(Systems::TextureAsset::GetAssetTypeNameSid());
		assetTypes.PushBack(Systems::CubemapAsset::GetAssetTypeNameSid());

		Core::Array<const Systems::AssetMetadata*> metadataArray;
		assetMgr.GetAssets(assetTypes, metadataArray);

		for (const Systems::AssetMetadata* pMetadata : metadataArray)
		{
			CachedTextureData cachedData;
			CreateCachedData(*pMetadata, cachedData);
			m_cache.PushBack(cachedData);
		}
	}

	TextureListModel::~TextureListModel()
	{ }

	Widgets::ModelIndex TextureListModel::GetParent(const Widgets::ModelIndex& index) const
	{
		return Widgets::ModelIndex(); //no parent in a flat list
	}

	Widgets::ModelIndex TextureListModel::GetIndex(int row, int column, const Widgets::ModelIndex& parent) const
	{
		if (parent.IsValid())
			return Widgets::ModelIndex(); //it's a flat list, only the root has children

		if(row < 0 || row >= static_cast<int>(m_cache.GetSize()))
			return Widgets::ModelIndex(); //out of bounds

		if(column < 0 || column >= Columns::Count)
			return Widgets::ModelIndex(); //out of bounds

		const CachedTextureData& cachedData = m_cache[row];
		return CreateIndex(row, column, &cachedData);
	}

	int TextureListModel::GetRowCount(const Widgets::ModelIndex& parent) const
	{
		if (parent.IsValid())
			return 0;

		return m_cache.GetSize();
	}

	int TextureListModel::GetColumnCount(const Widgets::ModelIndex& parent) const
	{
		if (parent.IsValid())
			return 0;

		return Columns::Count;
	}

	std::string TextureListModel::GetData(const Widgets::ModelIndex& index)
	{
		if (!index.IsValid())
			return "";

		const CachedTextureData* pCachedData = static_cast<const CachedTextureData*>(index.GetConstDataPointer());
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

		case Columns::Type:
		{
			switch (pCachedData->m_type)
			{
			case CachedTextureData::Texture:
				return "Texture";
				break;

			case CachedTextureData::Cubemap:
				return "Cubemap";
				break;

			default:
				return "Unknown";
				break;
			}
		}
		break;

		case Columns::Modified:
			return pCachedData->m_modified ? "*" : "";
			break;
		}

		return "__ERROR__";
	}

	std::string TextureListModel::GetHeaderData(int column)
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
		}

		return "__ERROR__";
	}

	void TextureListModel::AddRow(const Systems::AssetMetadata& metadata)
	{
		CachedTextureData cachedData;
		CreateCachedData(metadata, cachedData);
		
		int row = m_cache.GetSize();
		m_cache.PushBack(cachedData);

		CommitInsertRows(row, 1, Widgets::ModelIndex());
	}

	void TextureListModel::RemoveRow(Systems::NewAssetId id)
	{
		Core::Array<CachedTextureData>::Iterator it = std::find_if(m_cache.begin(), m_cache.end(), [id](const CachedTextureData& data) { return data.m_id == id; });
		if (it == m_cache.end())
			return;

		size_t index = std::distance(m_cache.begin(), it);
		int row = static_cast<int>(index);

		BeforeRemoveRows(row, 1, Widgets::ModelIndex());

		m_cache.Erase(it);

		AfterRemoveRows(row, 1, Widgets::ModelIndex());
	}

	void TextureListModel::SetTextureModified(Systems::NewAssetId id)
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

	void TextureListModel::ClearTextureModified(Systems::NewAssetId id)
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

	void TextureListModel::OnTextureRenamed(const Systems::AssetMetadata& metadata)
	{
		Widgets::ModelIndex index = GetIndex(metadata.GetAssetId());
		if (!index.IsValid())
			return;

		CachedTextureData& data = m_cache[index.GetRow()];
		data.m_virtualName = metadata.GetVirtualName();

		m_onDataChanged(index.GetSiblingAtColumn(Columns::Name));
	}

	Systems::NewAssetId TextureListModel::GetAssetId(const Widgets::ModelIndex& index) const
	{
		if (!index.IsValid())
			return Systems::NewAssetId::INVALID;

		if(index.GetParent().IsValid())
			return Systems::NewAssetId::INVALID;

		uint32_t row = static_cast<uint32_t>(index.GetRow());
		if(row >= m_cache.GetSize())
			return Systems::NewAssetId::INVALID;

		return m_cache[row].m_id;
	}

	Widgets::ModelIndex TextureListModel::GetIndex(Systems::NewAssetId id) const
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

	void TextureListModel::CreateCachedData(const Systems::AssetMetadata& metadata, CachedTextureData& cache) const
	{
		cache.m_id = metadata.GetAssetId();
		cache.m_virtualName = metadata.GetVirtualName();
		cache.m_modified = false;

		if (metadata.IsA<Systems::TextureAsset>())
			cache.m_type = CachedTextureData::Texture;
		else if (metadata.IsA<Systems::CubemapAsset>())
			cache.m_type = CachedTextureData::Cubemap;
		else
			cache.m_type = CachedTextureData::Unknown;
	}
}