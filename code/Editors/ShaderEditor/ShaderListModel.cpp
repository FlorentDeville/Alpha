/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/ShaderEditor/ShaderListModel.h"

#include "Editors/ShaderEditor/ShaderEditorModule.h"

#include "Systems/Assets/AssetMgr.h"

#include "Widgets/Models/ModelIndex.h"

namespace Editors
{
	enum Column
	{
		Id = 0,
		Name,
		Modified,

		Count
	};

	ShaderListModel::ShaderListModel()
		: AbstractViewModel()
	{
		const Systems::AssetMgr& assetMgr = Systems::AssetMgr::Get();
		const ShaderEditorModule& module = ShaderEditorModule::Get();
		const std::vector<Systems::NewAssetId>& allShaders = module.GetAllShaders();
		for (Systems::NewAssetId id : allShaders)
		{
			const Systems::AssetMetadata* pMetadata = assetMgr.GetMetadata(id);
			AddToCache(pMetadata);
		}
	}

	ShaderListModel::~ShaderListModel()
	{ }

	Widgets::ModelIndex ShaderListModel::GetParent(const Widgets::ModelIndex&) const
	{
		return Widgets::ModelIndex(); //there is no parent in arrays
	}

	Widgets::ModelIndex ShaderListModel::GetIndex(int row, int column, const Widgets::ModelIndex& parent) const
	{
		if (parent.IsValid())
			return Widgets::ModelIndex();

		if (row < 0 || row >= m_cache.size())
			return Widgets::ModelIndex();

		if (column < 0 || column >= Column::Count)
			return Widgets::ModelIndex();

		const CachedShaderData& data = m_cache[row];
		return CreateIndex(row, column, &data);
	}

	int ShaderListModel::GetRowCount(const Widgets::ModelIndex& parent) const
	{
		if (parent.IsValid())
			return 0;

		return static_cast<int>(m_cache.size());
	}

	int ShaderListModel::GetColumnCount(const Widgets::ModelIndex& parent) const
	{
		if (parent.IsValid())
			return 0;

		return Column::Count;
	}

	std::string ShaderListModel::GetData(const Widgets::ModelIndex& index)
	{
		const CachedShaderData* pData = reinterpret_cast<const CachedShaderData*>(index.GetConstDataPointer());
		switch (index.GetColumn())
		{
		case Column::Id:
			return pData->m_id.ToString();
			break;

		case Column::Name:
			return pData->m_virtualName;
			break;

		case Column::Modified:
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

	void ShaderListModel::AddRow(const Systems::AssetMetadata* pMetadata)
	{
		int row = static_cast<int>(m_cache.size());
		
		AddToCache(pMetadata);
		
		CommitInsertRows(row, 1, Widgets::ModelIndex());
	}

	void ShaderListModel::RemoveRow(Systems::NewAssetId id)
	{
		int cacheIndex = FindCacheIndex(id);
		if (cacheIndex < 0)
			return;

		std::vector<CachedShaderData>::const_iterator it = m_cache.cbegin() + cacheIndex;
		m_cache.erase(it);

		RemoveRows(cacheIndex, 1, Widgets::ModelIndex());
	}

	void ShaderListModel::SetShaderModified(Systems::NewAssetId id)
	{
		int cacheIndex = FindCacheIndex(id);
		if (cacheIndex == -1)
			return;

		if (m_cache[cacheIndex].m_modified)
			return;

		m_cache[cacheIndex].m_modified = true;
		m_onDataChanged(GetIndex(cacheIndex, Column::Modified, Widgets::ModelIndex()));
	}

	void ShaderListModel::ClearShaderModified(Systems::NewAssetId id)
	{
		int cacheIndex = FindCacheIndex(id);
		if (cacheIndex == -1)
			return;

		if (!m_cache[cacheIndex].m_modified)
			return;

		m_cache[cacheIndex].m_modified = false;
		m_onDataChanged(GetIndex(cacheIndex, Column::Modified, Widgets::ModelIndex()));
	}

	Systems::NewAssetId ShaderListModel::GetAssetId(const Widgets::ModelIndex& index) const
	{
		const CachedShaderData* pData = reinterpret_cast<const CachedShaderData*>(index.GetConstDataPointer());
		return pData->m_id;
	}

	Widgets::ModelIndex ShaderListModel::GetIndex(Systems::NewAssetId id) const
	{
		int index = FindCacheIndex(id);
		if(index == -1)
			return Widgets::ModelIndex();
		
		return GetIndex(index, 0, Widgets::ModelIndex());
	}

	void ShaderListModel::AddToCache(const Systems::AssetMetadata* pMetadata)
	{
		m_cache.push_back(CachedShaderData());
		CachedShaderData& data = m_cache.back();
		data.m_id = pMetadata->GetAssetId();
		data.m_virtualName = pMetadata->GetVirtualName();
		data.m_modified = false;
	}

	int ShaderListModel::FindCacheIndex(Systems::NewAssetId id) const
	{
		for (size_t ii = 0; ii < m_cache.size(); ++ii)
		{
			const CachedShaderData& data = m_cache[ii];
			if (data.m_id == id)
				return static_cast<int>(ii);
		}
		
		return -1;
	}
}
