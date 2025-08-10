/********************************************************************/
/* � 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/Widgets/PropertyGrid/PropertyGridPopulator.h"

#include "Core/Guid/Guid.h"
#include "Core/Math/Mat44f.h"
#include "Core/Math/Vec4f.h"

#include "Editors/Widgets/Dialog/AssetDialog.h"
#include "Editors/Widgets/Dialog/ClassSelectionDialog.h"
#include "Editors/Widgets/Dialog/OkCancelDialog.h"
#include "Editors/Widgets/PropertyGrid/Items/ArrayElementHeaderItem.h"
#include "Editors/Widgets/PropertyGrid/Items/ArrayHeaderItem.h"
#include "Editors/Widgets/PropertyGrid/Items/AssetIdItem.h"
#include "Editors/Widgets/PropertyGrid/Items/GuidItem.h"
#include "Editors/Widgets/PropertyGrid/Items/Mat44fItem.h"
#include "Editors/Widgets/PropertyGrid/Items/ObjectHeaderItem.h"
#include "Editors/Widgets/PropertyGrid/Items/SidItem.h"
#include "Editors/Widgets/PropertyGrid/Items/StringItem.h"
#include "Editors/Widgets/PropertyGrid/Items/UInt32Item.h"
#include "Editors/Widgets/PropertyGrid/PropertyGridItem.h"
#include "Editors/Widgets/PropertyGrid/PropertyGridItemFactory.h"
#include "Editors/Widgets/PropertyGrid/PropertyGridWidget.h"

#include "Systems/Assets/AssetMgr.h"
#include "Systems/Assets/NewAssetId.h"
#include "Systems/Objects/GameComponent.h"
#include "Systems/Objects/GameObject.h"
#include "Systems/Objects/Object.h"

#include "Widgets/WidgetMgr.h"

#include <charconv>
#include <stdio.h>

namespace Editors
{
	PropertyGridPopulator::PropertyGridPopulator()
		: m_pPropertyGridWidget(nullptr)
		, m_pObject(nullptr)
		, m_canAddElementToArray(true)
		, m_watcherCallbackIds()
		, m_propertyItemsTree()
		, m_propertyItemDepth()
	{
		m_parentItemContext.m_pParent = nullptr;
		m_parentItemContext.depth = 0;

		m_propertyItemDepth[nullptr] = 0;
	}

	PropertyGridPopulator::~PropertyGridPopulator()
	{
		for (const std::pair<const Systems::Object*, Core::CallbackId>& pair : m_watcherCallbackIds)
			ObjectWatcher::Get().RemoveWatcher(pair.first, pair.second);

		for (const std::pair<Core::Sid, PropertyGridItemFactory*>& pair : m_factories)
			delete pair.second;

		m_factories.clear();
	}

	void PropertyGridPopulator::Init(PropertyGridWidget* pWidget)
	{
		m_pPropertyGridWidget = pWidget;
	}

	void PropertyGridPopulator::Populate(Systems::Object* pObject)
	{
		for (const std::pair<const Systems::Object*, Core::CallbackId>& pair : m_watcherCallbackIds)
			ObjectWatcher::Get().RemoveWatcher(pair.first, pair.second);

		m_watcherCallbackIds.clear();
		m_parentItemContext.m_pParent = nullptr;
		m_parentItemContext.depth = 0;

		m_propertyItemDepth.clear();
		m_propertyItemsTree.clear();
		m_pObject = pObject;
		
		m_propertyItemDepth[nullptr] = 0;

		CreatePropertiesForObject(pObject);

		Widgets::WidgetMgr::Get().RequestResize();
	}

	void PropertyGridPopulator::Repopulate()
	{
		m_pPropertyGridWidget->ClearAllItems();
		Populate(m_pObject);
	}

	void PropertyGridPopulator::RegisterItemFactory(Core::Sid typenameSid, PropertyGridItemFactory* pFactory)
	{
		pFactory->Init(m_pPropertyGridWidget, this);

		if(PropertyGridItemFactory* pFactory = GetFactory(typenameSid))
			delete pFactory;
		
		m_factories[typenameSid] = pFactory;
	}

	void PropertyGridPopulator::SetCanAddElementToArray(bool canAdd)
	{
		m_canAddElementToArray = canAdd;
	}

	void PropertyGridPopulator::AddPropertyGridItem(PropertyGridItem* pItem)
	{
		Internal_AddPropertyGridItem(pItem);
	}

	void PropertyGridPopulator::CreatePropertiesForObject(Systems::Object* pObject)
	{
		Core::CallbackId callbackId = ObjectWatcher::Get().AddWatcher(pObject,
			[this](Systems::Object* pObj, const Systems::FieldDescriptor* pField, ObjectWatcher::OPERATION op, uint32_t index) { ObjectWatcherCallback(pObj, pField, op, index); });

		m_watcherCallbackIds[pObject] = callbackId;

		const Systems::TypeDescriptor* pRealType = pObject->GetTypeDescriptor();
		CreatePropertiesForObjectParentClass(pObject, pRealType);
	}

	void PropertyGridPopulator::CreatePropertiesForObjectParentClass(Systems::Object* pObject, const Systems::TypeDescriptor* pType)
	{
		const Systems::TypeDescriptor* pParentType = pType->GetBaseType();
		if (pParentType)
			CreatePropertiesForObjectParentClass(pObject, pParentType);
		
		CreatePropertiesForTypeMembers(pType, pObject);
	}

	void PropertyGridPopulator::CreatePropertiesForArrayElements(const Systems::FieldDescriptor* pField, void* pObj)
	{
		Core::BaseArray* pArray = pField->GetDataPtr<Core::BaseArray>(pObj);
		int32_t size = pArray->GetSize();

		for (int ii = 0; ii < size; ++ii)
		{
			CreatePropertiesForSingleArrayElement(pField, pObj, ii);
		}
	}

	void PropertyGridPopulator::CreatePropertiesForSingleArrayElement(const Systems::FieldDescriptor* pField, void* pObj, uint32_t index)
	{
		Core::BaseArray* pArray = pField->GetDataPtr<Core::BaseArray>(pObj);
		const Systems::TypeDescriptor* pArrayType = pField->GetType();
		const Systems::TypeDescriptor* pElementType = pArrayType->GetElementType();
		bool isObject = pElementType->IsObject();

		void* pElement = pArray->GetElement(index);
		if (pArrayType->IsElementPointer())
		{
			uint64_t* pCharPtr = reinterpret_cast<uint64_t*>(pElement);
			pElement = reinterpret_cast<uint64_t*>(*pCharPtr);
		}

		if (PropertyGridItemFactory* pFactory = GetFactory(pElementType->GetSid()))
		{
			pFactory->CreateItems(pElementType, pElement, 0);
		}
		else if (pElementType->IsContainer())
		{
			assert(false); //don't support array of arrays for now
		}
		else if (isObject)
		{
			ArrayElementHeaderItem* pItem = new ArrayElementHeaderItem(static_cast<Systems::Object*>(pObj), pField, index);
			Internal_AddPropertyGridItem(pItem, index);

			ParentItemContextScope janitor(pItem, this);

			Systems::Object* pObject = reinterpret_cast<Systems::Object*>(pElement);
			CreatePropertiesForObject(pObject);
		}
		else if (pElementType->IsClass())
		{
			assert(false); // only support Object for now
		}
		else //pod
		{
			PropertyGridItem* pItem = CreatePropertyItemForPODField(pField, pObj, index);
			Internal_AddPropertyGridItem(pItem, index);
		}
	}

	void PropertyGridPopulator::CreatePropertiesForTypeMembers(const Systems::TypeDescriptor* pFieldType, void* pData)
	{
		const std::vector<Systems::FieldDescriptor*>& members = pFieldType->GetFields();
		for (const Systems::FieldDescriptor* pField : members)
		{
			if (pField->IsHidden())
				continue;

			const Systems::TypeDescriptor* memberType = pField->GetType();

			void* pMemberPtr = pField->GetDataPtr(pData);
			if (pField->IsPointer())
			{
				char* pCharPtr = reinterpret_cast<char*>(pMemberPtr);
				pMemberPtr = reinterpret_cast<char*>(*pCharPtr);
			}

			if (memberType->IsContainer())
			{
				ArrayHeaderItem* pItem = new ArrayHeaderItem(reinterpret_cast<Systems::Object*>(pData), pField, -1, m_canAddElementToArray);
				Internal_AddPropertyGridItem(pItem);

				ParentItemContextScope janitor(pItem, this);

				CreatePropertiesForArrayElements(pField, pData);
			}
			else if (memberType->IsObject())
			{
				ObjectHeaderItem* pItem = new ObjectHeaderItem(reinterpret_cast<Systems::Object*>(pData), pField, 0);
				Internal_AddPropertyGridItem(pItem);

				ParentItemContextScope janitor(pItem, this);

				Systems::Object* pObject = reinterpret_cast<Systems::Object*>(pMemberPtr);
				CreatePropertiesForObject(pObject);
			}
			else if (memberType->IsClass())
			{
				PropertyGridItem* pItem = new PropertyGridItem(pField->GetName(), nullptr);
				Internal_AddPropertyGridItem(pItem);

				ParentItemContextScope janitor(pItem, this);

				CreatePropertiesForTypeMembers(memberType, pMemberPtr);
			}
			else //pod
			{
				PropertyGridItem* pItem = CreatePropertyItemForPODField(pField, pData, 0);
				Internal_AddPropertyGridItem(pItem);
			}
		}

	}

	PropertyGridItem* PropertyGridPopulator::CreatePropertyItemForPODField(const Systems::FieldDescriptor* pField, void* pObj, uint32_t indexElement)
	{
		const Systems::TypeDescriptor* pFieldType = pField->GetType();	
		
		if (pFieldType->IsContainer()) 
			pFieldType = pFieldType->GetElementType();

		switch (pFieldType->GetSid())
		{
		case SID("std::string"):
		{
			StringItem* pItem = new StringItem(static_cast<Systems::Object*>(pObj), pField, indexElement);
			return pItem;
		}
		break;

		case SID("uint32_t"):
		{
			UInt32Item* pItem = new UInt32Item(static_cast<Systems::Object*>(pObj), pField, indexElement);
			return pItem;
		}
		break;

		case SID("Core::Sid"):
		{
			SidItem* pItem = new SidItem(static_cast<Systems::Object*>(pObj), pField, indexElement);
			return pItem;
		}
		break;

		case SID("Core::Guid"):
		{
			GuidItem* pItem = new GuidItem(static_cast<Systems::Object*>(pObj), pField, indexElement);
			return pItem;
		}
		break;

		case SID("Core::Mat44f"):
		{
			Mat44fItem* pItem = new Mat44fItem(static_cast<Systems::Object*>(pObj), pField, indexElement);
			return pItem;
		}
		break;

		case SID("Systems::NewAssetId"):
		{
			AssetIdItem* pItem = new AssetIdItem(static_cast<Systems::Object*>(pObj), pField, indexElement);
			return pItem;
		}
		break;

		default:
			assert(false); //unsupported type
			break;
		}

		return nullptr;
	}

	void PropertyGridPopulator::Internal_AddPropertyGridItem(PropertyGridItem* pItem)
	{
		PropertyGridItem* pInsertAfter = nullptr;
		{
			pInsertAfter = m_parentItemContext.m_pParent;
			while (m_propertyItemsTree[pInsertAfter].GetSize() != 0)
			{
				pInsertAfter = m_propertyItemsTree[pInsertAfter].Back();
			}
		}
		
		uint32_t depth = m_propertyItemDepth[m_parentItemContext.m_pParent] + 1;

		m_pPropertyGridWidget->InsertProperty(pItem, pInsertAfter, depth);
		m_propertyItemsTree[m_parentItemContext.m_pParent].PushBack(pItem);
		m_propertyItemParent[pItem] = m_parentItemContext.m_pParent;
		m_propertyItemDepth[pItem] = depth;
	}

	void PropertyGridPopulator::Internal_AddPropertyGridItem(PropertyGridItem* pItem, uint32_t index)
	{
		Core::Array<PropertyGridItem*>& children = m_propertyItemsTree[m_parentItemContext.m_pParent];
		if (!children.IsValidIndex(index)) //if the index is not valid, add at the end.
		{
			Internal_AddPropertyGridItem(pItem);
			return;
		}
		
		PropertyGridItem* pInsertAfter = nullptr;
		{
			PropertyGridItem* pInsertAfter = children[index - 1];
			while (m_propertyItemsTree[pInsertAfter].GetSize() != 0)
			{
				pInsertAfter = m_propertyItemsTree[pInsertAfter].Back();
			}
		}

		uint32_t depth = m_propertyItemDepth[m_parentItemContext.m_pParent] + 1;

		m_pPropertyGridWidget->InsertProperty(pItem, pInsertAfter, depth);
		m_propertyItemsTree[m_parentItemContext.m_pParent].PushBack(pItem);
		m_propertyItemParent[pItem] = m_parentItemContext.m_pParent;
		m_propertyItemDepth[pItem] = depth;
	}		

	void PropertyGridPopulator::DeletePropertyGridItemRecursively(PropertyGridItem* pItemToDelete)
	{
		std::map<const PropertyGridItem*, Core::Array<PropertyGridItem*>>::iterator it = m_propertyItemsTree.find(pItemToDelete);
		if (it == m_propertyItemsTree.end())
			return;

		Core::Array<PropertyGridItem*>& children = it->second;
		for (PropertyGridItem* pChild : children)
		{
			DeletePropertyGridItemRecursively(pChild);
			m_pPropertyGridWidget->RemoveProperty(pChild);
			
			m_propertyItemParent.erase(pChild);
		}

		m_propertyItemsTree.erase(pItemToDelete);
	}

	PropertyGridItemFactory* PropertyGridPopulator::GetFactory(Core::Sid typeSid) const
	{
		std::map<Core::Sid, PropertyGridItemFactory*>::const_iterator it = m_factories.find(typeSid);
		if (it != m_factories.cend())
			return it->second;

		return nullptr;
	}

	void PropertyGridPopulator::ObjectWatcherCallback(Systems::Object* pObj, const Systems::FieldDescriptor* pField, ObjectWatcher::OPERATION op, uint32_t index)
	{
		Core::Array<PropertyGridItem*> items = m_pPropertyGridWidget->GetPropertyGridItems();
		Core::Array<PropertyGridItem*>::Iterator it = std::find_if(items.begin(), items.end(), [pObj, pField, index](PropertyGridItem* item) 
			{ return item->IsField(pObj, pField, index); });

		if (op != ObjectWatcher::ADD_ELEMENT)
		{
			if (it == items.end())
				return;
		}

		switch (op)
		{
		case ObjectWatcher::SET_FIELD:
		case ObjectWatcher::SET_ELEMENT:
			(*it)->UpdateValue();
			break;

		case ObjectWatcher::ADD_ELEMENT:
		{
			//Trying to just add a new element is getting complicated so for now just repopulate the widget.
			//It's simple and it works. I'll improve it if necessary later. No need to waste more time on this now.
			//Repopulate();
		}
			break;

		case ObjectWatcher::REMOVE_ELEMENT:
		{
			PropertyGridItem* pItem = *it;
			DeletePropertyGridItemRecursively(pItem);

			PropertyGridItem* pParent = m_propertyItemParent[pItem];
			m_propertyItemsTree[pParent].Erase(pItem);
			m_propertyItemParent.erase(pItem);
			m_propertyItemDepth.erase(pItem);

			m_pPropertyGridWidget->RemoveProperty(pItem);

			//update the index of all the properties for the element of the array
		}
			break;
		}
	}

	PropertyGridPopulator::ParentItemContextScope::ParentItemContextScope(PropertyGridItem* m_pParent, PropertyGridPopulator* pPopulator)
		: m_previous(pPopulator->m_parentItemContext)
		, m_pPopulator(pPopulator)
	{
		pPopulator->m_parentItemContext.m_pParent = m_pParent;
		++pPopulator->m_parentItemContext.depth;
	}

	PropertyGridPopulator::ParentItemContextScope::~ParentItemContextScope()
	{
		m_pPopulator->m_parentItemContext = m_previous;
	}
}
