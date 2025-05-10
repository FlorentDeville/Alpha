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

	int ShaderListModel::GetRowCount(const Widgets::ModelIndex& index)
	{
		return static_cast<int>(m_cache.size());
	}

	int ShaderListModel::GetColumnCount(const Widgets::ModelIndex&) const
	{
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

	Systems::NewAssetId ShaderListModel::GetAssetId(const Widgets::ModelIndex& index) const
	{
		const CachedShaderData* pData = reinterpret_cast<const CachedShaderData*>(index.GetConstDataPointer());
		return pData->m_id;
	}

	void ShaderListModel::AddToCache(const Systems::AssetMetadata* pMetadata)
	{
		m_cache.push_back(CachedShaderData());
		CachedShaderData& data = m_cache.back();
		data.m_id = pMetadata->GetAssetId();
		data.m_virtualName = pMetadata->GetVirtualName();
		data.m_modified = false;
	}
}
