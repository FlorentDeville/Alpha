/********************************************************************/
/* © 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/TextureEditor/TextureListModel.h"

#include "Core/Collections/Array.h"

#include "Systems/Assets/AssetMgr.h"
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

		Core::Array<const Systems::AssetMetadata*> metadataArray;
		assetMgr.GetAssets(Systems::TextureAsset::GetAssetTypeNameSid(), metadataArray);

		for (const Systems::AssetMetadata* pMetadata : metadataArray)
		{
			CachedTextureData cachedData;
			cachedData.m_id = pMetadata->GetAssetId();
			cachedData.m_virtualName = pMetadata->GetVirtualName();
			cachedData.m_modified = false;

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

		case Columns::Modified:
			return "Modified";
			break;
		}

		return "__ERROR__";
	}

	void TextureListModel::AddRow(const Systems::AssetMetadata& metadata)
	{
		if (!metadata.IsA<Systems::TextureAsset>())
			return;

		CachedTextureData cachedData;
		cachedData.m_id = metadata.GetAssetId();
		cachedData.m_virtualName = metadata.GetVirtualName();
		cachedData.m_modified = false;

		int row = m_cache.GetSize();
		m_cache.PushBack(cachedData);

		CommitInsertRows(row, 1, Widgets::ModelIndex());
	}

	/*void TextureListModel::RemoveRow(Systems::NewAssetId id);

	void TextureListModel::SetTextureModified(Systems::NewAssetId id);
	void TextureListModel::ClearTextureModified(Systems::NewAssetId id);

	void TextureListModel::OnTextureRenamed(const Systems::AssetMetadata& metadata);*/
}