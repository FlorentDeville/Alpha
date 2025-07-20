/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/LevelEditor/SceneTreeModel.h"

#include "Systems/Assets/AssetMgr.h"
#include "Systems/Assets/AssetObjects/Level/LevelAsset.h"

#include "Widgets/Models/ModelIndex.h"

#include <assert.h>

namespace Editors
{
	SceneTreeModel::SceneTreeModel(const Systems::LevelAsset* pLevel)
		: AbstractViewModel()
	{
		const Core::Array<Systems::GameObject*>& goArray = pLevel->GetGameObjectsArray();
		for (const Systems::GameObject* pGo : goArray)
		{
			CreateCachedItem(pGo);
		}
	}

	SceneTreeModel::~SceneTreeModel()
	{
		for (CachedItem* pItem : m_cachedItemArray)
			delete pItem;
	}

	Widgets::ModelIndex SceneTreeModel::GetParent(const Widgets::ModelIndex& index) const
	{
		if (index.GetColumn() != 0) //only column 0 has child/parent relationship
			return Widgets::ModelIndex();

		if(!index.IsValid())
			return Widgets::ModelIndex();

		const CachedItem* pItem = reinterpret_cast<const CachedItem*>(index.GetConstDataPointer());
		if(!pItem)
			return Widgets::ModelIndex();

		if (!pItem->m_parent.IsValid()) //no parent then I'm a root game object
			return Widgets::ModelIndex();

		const CachedItem* pParentItem = FindCachedItem(pItem->m_parent);
		if (!pParentItem)
			return Widgets::ModelIndex();

		//now I need to find my row
		if (!pParentItem->m_parent.IsValid()) //pParentItem is a root item
		{
			Core::Guid parentGuid = pParentItem->m_guid;
			Core::Array<CachedItem*>::Iterator it = std::find_if(m_cachedItemRootsArray.cbegin(), m_cachedItemRootsArray.cend(), [&parentGuid](const CachedItem* pItem) { return pItem->m_guid == parentGuid; });
			if (it == m_cachedItemRootsArray.cend())
				return Widgets::ModelIndex();

			size_t row = std::distance(m_cachedItemRootsArray.cbegin(), it);
			return CreateIndex(static_cast<int>(row), index.GetColumn(), pParentItem);
		}
		else
		{
			const CachedItem* pGrandParent = FindCachedItem(pParentItem->m_parent);
			if (!pGrandParent)
				return Widgets::ModelIndex();

			const Core::Array<Core::Guid>& childrenArray = pGrandParent->m_children;
			Core::Array<Core::Guid>::Iterator it = std::find(childrenArray.cbegin(), childrenArray.cend(), pParentItem->m_guid);
			if (it == childrenArray.cend())
				return Widgets::ModelIndex();

			size_t row = std::distance(childrenArray.cbegin(), it);
			return CreateIndex(static_cast<int>(row), index.GetColumn(), pParentItem);
		}
	}

	Widgets::ModelIndex SceneTreeModel::GetIndex(int row, int column, const Widgets::ModelIndex& parent) const
	{
		if (column < 0 || column >= Columns::Count) //invalid column
			return Widgets::ModelIndex();

		if (!parent.IsValid()) //no parent then we're asking for a root game object
		{
			if (!m_cachedItemRootsArray.IsValidIndex(row))
				return Widgets::ModelIndex();

			return CreateIndex(row, column, m_cachedItemRootsArray[row]);
		}

		const CachedItem* pCachedItem = reinterpret_cast<const CachedItem*>(parent.GetConstDataPointer());
		if (!pCachedItem)
			return Widgets::ModelIndex();

		if(!pCachedItem->m_children.IsValidIndex(row))
			return Widgets::ModelIndex();

		Core::Guid childGuid = pCachedItem->m_children[row];
		const CachedItem* pChildItem = FindCachedItem(childGuid);
		if(!pChildItem)
			return Widgets::ModelIndex();

		return CreateIndex(row, column, pChildItem);
	}

	int SceneTreeModel::GetRowCount(const Widgets::ModelIndex& parent) const
	{
		if (!parent.IsValid())
			return m_cachedItemRootsArray.GetSize();
		
		const CachedItem* pItem = reinterpret_cast<const CachedItem*>(parent.GetConstDataPointer());
		if (!pItem)
			return 0;

		return pItem->m_children.GetSize();
	}

	int SceneTreeModel::GetColumnCount(const Widgets::ModelIndex& parent) const
	{
		if (!parent.IsValid())
			return Columns::Count;

		if (parent.GetColumn() != 0)
			return 0;

		return Columns::Count;
	}

	std::string SceneTreeModel::GetData(const Widgets::ModelIndex& index)
	{
		const CachedItem* pItem = index.GetConstDataPointer<CachedItem>();
		if (!pItem)
			return "__ERROR__";

		switch (index.GetColumn())
		{
		case Columns::Guid:
		{
			const int BUFFER_SIZE = 64;
			char buffer[BUFFER_SIZE] = { '\0' };
			pItem->m_guid.ToString(buffer, BUFFER_SIZE);
			return buffer;
		}
		break;

		case Columns::Name:
		{
			return pItem->m_name;
		}
		break;

		case Columns::Type:
		{
			return pItem->m_type;
		}
		break;

		case Columns::Modified:
		{
			return "";
		}
		break;

		default:
			assert(false);
			break;
		}
		
		return "__ERROR__";
	}

	std::string SceneTreeModel::GetHeaderData(int column)
	{
		switch (column)
		{
		case Columns::Guid:
			return "Guid";
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

	void SceneTreeModel::CreateCachedItem(const Systems::GameObject* pGo)
	{
		CachedItem* pItem = new CachedItem();
		pItem->m_guid = pGo->GetGuid();
		pItem->m_name = pGo->GetName();
		pItem->m_type = pGo->GetTypeDescriptor()->GetName();
		pItem->m_parent = pGo->GetTransform().GetParentGuid();
		pItem->m_children = pGo->GetTransform().GetChildrenGuid();

		m_cachedItemArray.PushBack(pItem);
		m_cachedItemMap[pItem->m_guid] = pItem;

		if (!pItem->m_parent.IsValid())
			m_cachedItemRootsArray.PushBack(pItem);
	}

	const SceneTreeModel::CachedItem* SceneTreeModel::FindCachedItem(const Core::Guid& guid) const
	{
		std::map<Core::Guid, CachedItem*>::const_iterator it = m_cachedItemMap.find(guid);
		if (it == m_cachedItemMap.cend())
			return nullptr;

		return it->second;
	}
}
