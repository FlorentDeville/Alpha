/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/LevelEditor/SceneTreeModel.h"

#include "Editors/ObjectWatcher/ObjectWatcher.h"

#include "Systems/Assets/AssetMgr.h"
#include "Systems/Assets/AssetObjects/Level/LevelAsset.h"
#include "Systems/Objects/GameObject.h"

#include "Widgets/Models/ModelIndex.h"
#include "Widgets/Models/SelectionModel.h"
#include "Widgets/Models/SelectionRow.h"

#include <assert.h>

namespace Editors
{
	SceneTreeModel::SceneTreeModel(const Systems::LevelAsset* pLevel)
		: AbstractViewModel()
		, m_pLevel(pLevel)
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

	void SceneTreeModel::AddGameObject(const Systems::GameObject* pGo, const Systems::GameObject* pGoParent)
	{
		AddGameObject_Recursive(pGo, pGoParent);
	}

	void SceneTreeModel::RemoveGameObject(const Core::Guid& guid)
	{
		const CachedItem* pItem = FindCachedItem(guid);
		if (!pItem)
			return;

		Widgets::ModelIndex modelIndex = GetModelIndex(pItem);
		int row = modelIndex.GetRow();
		Widgets::ModelIndex parentModelIndex = modelIndex.GetParent();

		BeforeRemoveRows(row, 1, parentModelIndex);

		ObjectWatcher::Get().RemoveWatcher(pItem->m_objWatcherCid);

		Core::Guid parentGuid = pItem->m_parent;

		//delete the cache
		if (!parentGuid.IsValid())
		{
			m_cachedItemRootsArray.Erase(pItem);
		}
		else
		{
			if (CachedItem* pParentCachedItem = FindCachedItem(parentGuid))
			{
				pParentCachedItem->m_children.Erase(guid);
			}

		}

		m_cachedItemMap.erase(pItem->m_guid);
		m_cachedItemArray.Erase(pItem);

		delete pItem;

		AfterRemoveRows(row, 1, parentModelIndex);
	}

	void SceneTreeModel::RenameGameObject(const Core::Guid& guid, const std::string& newName)
	{
		CachedItem* pItem = FindCachedItem(guid);
		if (!pItem)
			return;

		pItem->m_name = newName;

		Widgets::ModelIndex index = GetModelIndex(pItem);
		if (!index.IsValid())
			return;

		Widgets::ModelIndex cellIndex = index.GetSiblingAtColumn(Columns::Name);
		m_onDataChanged(cellIndex);
	}

	void SceneTreeModel::ReparentGameObject(const Systems::GameObject* pGo, const Systems::GameObject* pOldParent, const Systems::GameObject* pNewParent)
	{
		RemoveGameObject(pGo->GetGuid());

		AddGameObject(pGo, pNewParent);
	}

	Core::Guid SceneTreeModel::FindGameObject(const Widgets::ModelIndex& index) const
	{
		const CachedItem* pItem = index.GetConstDataPointer<CachedItem>();
		if (!pItem)
			return Core::Guid();

		return pItem->m_guid;
	}

	void SceneTreeModel::ClearSelection()
	{
		if (Widgets::SelectionModel* pSelection = GetSelectionModel())
			pSelection->ClearSelectedRows();
	}

	void SceneTreeModel::SelectGameObject(const Core::Guid& guid)
	{
		const CachedItem* pItem = FindCachedItem(guid);
		if (!pItem)
			return;

		Widgets::ModelIndex startIndex = GetModelIndex(pItem);
		Widgets::ModelIndex endIndex = startIndex.GetSiblingAtColumn(Columns::Count - 1);

		Widgets::SelectionRow row(startIndex, endIndex);
		Widgets::SelectionModel* pSelection = GetSelectionModel();
		pSelection->SetSelectionRow(row);
	}

	void SceneTreeModel::DeselectGameObject(const Core::Guid& guid)
	{
		const CachedItem* pItem = FindCachedItem(guid);
		if (!pItem)
			return;

		Widgets::ModelIndex startIndex = GetModelIndex(pItem);
		Widgets::ModelIndex endIndex = startIndex.GetSiblingAtColumn(Columns::Count - 1);

		Widgets::SelectionRow row(startIndex, endIndex);
		Widgets::SelectionModel* pSelection = GetSelectionModel();
		pSelection->DeselectRow(row);
	}

	const SceneTreeModel::CachedItem* SceneTreeModel::CreateCachedItem(const Systems::GameObject* pGo)
	{
		CachedItem* pItem = new CachedItem();
		pItem->m_guid = pGo->GetGuid();
		pItem->m_name = pGo->GetName();
		pItem->m_type = pGo->GetTypeDescriptor()->GetName();
		pItem->m_parent = pGo->GetTransform().GetParentGuid();
		pItem->m_children = pGo->GetTransform().GetChildrenGuid();

		pItem->m_objWatcherCid = ObjectWatcher::Get().AddWatcher(pGo, [this](Systems::Object* pObj, const Systems::FieldDescriptor* pField, ObjectWatcher::OPERATION op, uint32_t index)
			{
				if (pField->GetName() == "m_name") // it's ugly to hardcode the name of the field here.
				{
					Systems::GameObject* pGo = static_cast<Systems::GameObject*>(pObj);
					RenameGameObject(pGo->GetGuid(), *pField->GetDataPtr<std::string>(pObj));
				}
			});

		m_cachedItemArray.PushBack(pItem);
		m_cachedItemMap[pItem->m_guid] = pItem;

		if (!pItem->m_parent.IsValid())
			m_cachedItemRootsArray.PushBack(pItem);

		return pItem;
	}

	SceneTreeModel::CachedItem* SceneTreeModel::FindCachedItem(const Core::Guid& guid)
	{
		std::map<Core::Guid, CachedItem*>::const_iterator it = m_cachedItemMap.find(guid);
		if (it == m_cachedItemMap.cend())
			return nullptr;

		return it->second;
	}

	const SceneTreeModel::CachedItem* SceneTreeModel::FindCachedItem(const Core::Guid& guid) const
	{
		std::map<Core::Guid, CachedItem*>::const_iterator it = m_cachedItemMap.find(guid);
		if (it == m_cachedItemMap.cend())
			return nullptr;

		return it->second;
	}

	Widgets::ModelIndex SceneTreeModel::GetModelIndex(const SceneTreeModel::CachedItem* pItem) const
	{
		if (!pItem->m_parent.IsValid()) //no parent then we're asking for a root game object
		{
			const Core::Guid& guid = pItem->m_guid;
			Core::Array<CachedItem*>::Iterator it = std::find_if(m_cachedItemRootsArray.cbegin(), m_cachedItemRootsArray.cend(), [&guid](const CachedItem* pItem) { return pItem->m_guid == guid; });
			if (it == m_cachedItemRootsArray.cend())
				return Widgets::ModelIndex();

			size_t row = std::distance(m_cachedItemRootsArray.cbegin(), it);
			return CreateIndex(static_cast<int>(row), 0, pItem);
		}

		const CachedItem* pParentItem = FindCachedItem(pItem->m_parent);
		if (!pParentItem)
			return Widgets::ModelIndex();

		const Core::Array<Core::Guid>& childrenArray = pParentItem->m_children;
		Core::Array<Core::Guid>::Iterator it = std::find(childrenArray.cbegin(), childrenArray.cend(), pItem->m_guid);
		if(it == childrenArray.cend())
			return Widgets::ModelIndex();

		size_t row = std::distance(childrenArray.cbegin(), it);
		
		return CreateIndex(static_cast<int>(row), 0, pItem);
	}

	void SceneTreeModel::AddGameObject_Recursive(const Systems::GameObject* pGo, const Systems::GameObject* pGoParent)
	{
		if (pGoParent)
		{
			if (CachedItem* pParentCachedItem = FindCachedItem(pGoParent->GetGuid()))
			{
				pParentCachedItem->m_children = pGoParent->GetTransform().GetChildrenGuid(); //copy everything to keep the order
			}
		}

		const CachedItem* pItem = CreateCachedItem(pGo);
		Widgets::ModelIndex index = GetModelIndex(pItem);
		CommitInsertRows(index.GetRow(), 1, index.GetParent());

		//now add the children
		const Core::Array<Core::Guid>& children = pGo->GetTransform().GetChildrenGuid();
		for (const Core::Guid& childGuid : children)
		{
			const Systems::GameObject* pGoChild = m_pLevel->FindGameObject(childGuid);
			if (!pGoChild)
				continue;

			AddGameObject_Recursive(pGoChild, pGo);
		}
	}
}
