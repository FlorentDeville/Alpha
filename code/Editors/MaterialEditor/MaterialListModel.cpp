/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/MaterialEditor/MaterialListModel.h"

#include "Editors/MaterialEditor/MaterialEditorModule.h"

#include "Systems/Assets/AssetMgr.h"
#include "Systems/Assets/AssetObjects/AssetUtil.h"
#include "Systems/Assets/AssetObjects/MaterialInstance/MaterialInstanceAsset.h"

#include "Widgets/Models/ModelIndex.h"

namespace Editors
{
	static const char* MISSING_BASE_MATERIAL_LABEL = "__MISSING__";

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

		const CachedShaderData& data = m_cache[row];
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
		const CachedShaderData* pData = reinterpret_cast<const CachedShaderData*>(index.GetConstDataPointer());
		switch (index.GetColumn())
		{
		case Columns::Id:
			return pData->m_id.ToString();
			break;

		case Columns::Name:
			return pData->m_virtualName;
			break;

		case Columns::Base:
			return pData->m_baseMaterial;
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

		case Columns::Base:
			return "Base";
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

		std::vector<CachedShaderData>::const_iterator it = m_cache.cbegin() + cacheIndex;
		m_cache.erase(it);

		Systems::AssetMetadata* pMetadata = Systems::AssetMgr::Get().GetMetadata(id);
		if(pMetadata)
		{
			if (pMetadata->IsA<Systems::MaterialInstanceAsset>())
			{
				Systems::MaterialInstanceAsset* pMaterialInstance = Systems::AssetUtil::LoadAsset<Systems::MaterialInstanceAsset>(id);
				if (pMaterialInstance)
				{
					std::map<Systems::NewAssetId, Core::Array<Systems::NewAssetId>>::iterator it = m_baseToInstance.find(pMaterialInstance->GetBaseMaterialId());
					if (it != m_baseToInstance.end())
					{
						it->second.Erase(id);
					}
				}
			}
		}

		AfterRemoveRows(cacheIndex, 1, Widgets::ModelIndex());

		//update the other rows after deletion so the indices are correct
		if (pMetadata->IsA<Systems::MaterialAsset>())
		{
			std::map<Systems::NewAssetId, Core::Array<Systems::NewAssetId>>::iterator it = m_baseToInstance.find(pMetadata->GetAssetId());
			if (it != m_baseToInstance.end())
			{
				for (Systems::NewAssetId instanceId : it->second)
				{
					int cacheIndex = FindCacheIndex(instanceId);
					if (cacheIndex == -1)
						continue;

					m_cache[cacheIndex].m_baseMaterial = MISSING_BASE_MATERIAL_LABEL;

					Widgets::ModelIndex instanceModelIndex = GetIndex(instanceId);
					instanceModelIndex = instanceModelIndex.GetSiblingAtColumn(Columns::Base);
					m_onDataChanged(instanceModelIndex);
				}
			}
		}
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

		//now update the base name for the material instance
		std::map<Systems::NewAssetId, Core::Array<Systems::NewAssetId>>::iterator it = m_baseToInstance.find(metadata.GetAssetId());
		if (it != m_baseToInstance.end())
		{
			Core::Array<Systems::NewAssetId>& materialInstanceArray = it->second;
			for (const Systems::NewAssetId materialInstanceId : materialInstanceArray)
			{
				int materialInstanceCacheIndex = FindCacheIndex(materialInstanceId);
				if (materialInstanceCacheIndex == -1)
					continue;

				m_cache[materialInstanceCacheIndex].m_baseMaterial = metadata.GetVirtualName();
				Widgets::ModelIndex materialInstanceModelIndex = GetIndex(materialInstanceId);
				materialInstanceModelIndex = materialInstanceModelIndex.GetSiblingAtColumn(Columns::Base);
				m_onDataChanged(materialInstanceModelIndex);
			}
		}
	}

	Systems::NewAssetId MaterialListModel::GetAssetId(const Widgets::ModelIndex& index) const
	{
		const CachedShaderData* pData = reinterpret_cast<const CachedShaderData*>(index.GetConstDataPointer());
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
		m_cache.push_back(CachedShaderData());
		CachedShaderData& data = m_cache.back();
		data.m_id = pMetadata->GetAssetId();
		data.m_virtualName = pMetadata->GetVirtualName();
		data.m_modified = false;
		data.m_baseMaterial = "";

		if (pMetadata->IsA<Systems::MaterialInstanceAsset>())
		{
			Systems::MaterialInstanceAsset* pAsset = Systems::AssetUtil::LoadAsset<Systems::MaterialInstanceAsset>(pMetadata->GetAssetId());
			if (pAsset)
			{
				Systems::NewAssetId baseId = pAsset->GetBaseMaterialId();
				if (const Systems::AssetMetadata* pBaseMetadata = Systems::AssetMgr::Get().GetMetadata(baseId))
				{
					data.m_baseMaterial = pBaseMetadata->GetVirtualName();
					m_baseToInstance[baseId].PushBack(pMetadata->GetAssetId());
				}
				else
				{
					data.m_baseMaterial = MISSING_BASE_MATERIAL_LABEL;
				}
			}
		}
	}

	int MaterialListModel::FindCacheIndex(Systems::NewAssetId id) const
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
