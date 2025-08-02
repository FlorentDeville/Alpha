/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/Widgets/PropertyGrid/PropertyGridPopulator.h"

#include "Core/Guid/Guid.h"
#include "Core/Math/Mat44f.h"
#include "Core/Math/Vec4f.h"

#include "Editors/Widgets/Dialog/AssetDialog.h"
#include "Editors/Widgets/Dialog/ClassSelectionDialog.h"
#include "Editors/Widgets/Dialog/OkCancelDialog.h"
#include "Editors/Widgets/PropertyGrid/Items/AssetIdItem.h"
#include "Editors/Widgets/PropertyGrid/Items/GuidItem.h"
#include "Editors/Widgets/PropertyGrid/Items/Mat44fItem.h"
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

#include "Widgets/Button.h"
#include "Widgets/Icon.h"
#include "Widgets/Label.h"
#include "Widgets/Layout.h"
#include "Widgets/TextBox.h"
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
	{ }

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

		m_pObject = pObject;
		
		CreatePropertiesForObject(pObject, 0);

		m_watcherCallbackId = ObjectWatcher::Get().AddWatcher(pObject, 
			[this](Systems::Object* pObj, const Systems::FieldDescriptor* pField, ObjectWatcher::OPERATION op, uint32_t index) { ObjectWatcherCallback(pObj, pField, op, index); });

		Widgets::WidgetMgr::Get().RequestResize();
	}

	void PropertyGridPopulator::Repopulate()
	{
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

	void PropertyGridPopulator::SendDataChangedEvent()
	{
		m_onDataChanged();
	}

	void PropertyGridPopulator::CreatePropertiesForObject(Systems::Object* pObject, int depth)
	{
		Core::CallbackId callbackId = ObjectWatcher::Get().AddWatcher(pObject,
			[this](Systems::Object* pObj, const Systems::FieldDescriptor* pField, ObjectWatcher::OPERATION op, uint32_t index) { ObjectWatcherCallback(pObj, pField, op, index); });

		m_watcherCallbackIds[pObject] = callbackId;

		const Systems::TypeDescriptor* pRealType = pObject->GetTypeDescriptor();
		CreatePropertiesForObjectParentClass(pObject, pRealType, depth);
	}

	void PropertyGridPopulator::CreatePropertiesForObjectParentClass(Systems::Object* pObject, const Systems::TypeDescriptor* pType, int depth)
	{
		const Systems::TypeDescriptor* pParentType = pType->GetBaseType();
		if (pParentType)
			CreatePropertiesForObjectParentClass(pObject, pParentType, depth);
		
		CreatePropertiesForTypeMembers(pType, pObject, depth);
	}

	void PropertyGridPopulator::CreatePropertiesForArrayElements(const Systems::FieldDescriptor* pField, void* pObj, int depth)
	{
		Core::BaseArray* pArray = pField->GetDataPtr<Core::BaseArray>(pObj);
		const Systems::TypeDescriptor* pArrayType = pField->GetType();

		int32_t size = pArray->GetSize();

		const Systems::TypeDescriptor* pElementType = pArrayType->GetElementType();
		bool isObject = pElementType->IsObject();

		for (int ii = 0; ii < size; ++ii)
		{
			void* pElement = pArray->GetElement(ii);
			if (pArrayType->IsElementPointer())
			{
				uint64_t* pCharPtr = reinterpret_cast<uint64_t*>(pElement);
				pElement = reinterpret_cast<uint64_t*>(*pCharPtr);
			}

			if (PropertyGridItemFactory* pFactory = GetFactory(pElementType->GetSid()))
			{
				pFactory->CreateItems(pElementType, pElement, depth + 1);
			}
			else if (pElementType->IsContainer())
			{
				assert(false); //don't support array of arrays for now
			}
			else if (isObject)
			{
				Widgets::Widget* pNameLayout = CreateArrayItemName(pArray, ii, isObject, pElement);
				PropertyGridItem* pItem = new PropertyGridItem(pNameLayout, nullptr);
				m_pPropertyGridWidget->AddProperty(pItem, depth);

				Systems::Object* pObject = reinterpret_cast<Systems::Object*>(pElement);
				CreatePropertiesForObject(pObject, depth + 1);
			}
			else if (pElementType->IsClass())
			{
				Widgets::Widget* pNameLayout = CreateArrayItemName(pArray, ii, isObject, pElement);
				PropertyGridItem* pItem = new PropertyGridItem(pNameLayout, nullptr);
				m_pPropertyGridWidget->AddProperty(pItem, depth);

				CreatePropertiesForTypeMembers(pElementType, pElement, depth + 1);
			}
			else //pod
			{
				PropertyGridItem* pItem = CreatePropertyItemForPODField(pField, pObj, ii);
				m_pPropertyGridWidget->AddProperty(pItem, depth);
			}
		}
	}

	void PropertyGridPopulator::CreatePropertiesForTypeMembers(const Systems::TypeDescriptor* pFieldType, void* pData, int depth)
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
				PropertyGridItem* pItem = new PropertyGridItem(pField->GetName(), nullptr);
				m_pPropertyGridWidget->AddProperty(pItem, depth);

				CreatePropertiesForArrayElements(pField, pData, depth + 1);
				
				if(m_canAddElementToArray)
					CreateArrayAddElementButton(*pField, pMemberPtr);
			}
			else if (memberType->IsObject())
			{
				Systems::Object* pObject = reinterpret_cast<Systems::Object*>(pMemberPtr);
				CreatePropertiesForObject(pObject, depth + 1);
			}
			else if (memberType->IsClass())
			{
				PropertyGridItem* pItem = new PropertyGridItem(pField->GetName(), nullptr);
				m_pPropertyGridWidget->AddProperty(pItem);

				CreatePropertiesForTypeMembers(memberType, pMemberPtr, depth + 1);
			}
			else //pod
			{
				PropertyGridItem* pItem = CreatePropertyItemForPODField(pField, pData, 0);
				m_pPropertyGridWidget->AddProperty(pItem, depth);
			}
		}

	}

	void PropertyGridPopulator::CreateArrayAddElementButton(const Systems::FieldDescriptor& member, void* pMemberPtr)
	{
		Widgets::Label* pLabel = new Widgets::Label("+");
		pLabel->SetSizeStyle(Widgets::Widget::FIT);
		pLabel->SetPositionStyle(Widgets::Widget::HPOSITION_STYLE::CENTER, Widgets::Widget::VPOSITION_STYLE::MIDDLE);

		Widgets::Button* pAddElementButton = new Widgets::Button(10, 20, 0, 0);
		pAddElementButton->AddWidget(pLabel);
		pAddElementButton->SetSizeStyle(Widgets::Widget::HSIZE_STRETCH | Widgets::Widget::VSIZE_DEFAULT);
		pAddElementButton->OnClick([this, member, pMemberPtr]()
			{
				const Systems::TypeDescriptor* pArrayType = member.GetType();
				if (pArrayType->IsElementPointer())
				{
					const Systems::TypeDescriptor* pElementType = pArrayType->GetElementType();

					const Systems::TypeDescriptor* pElementBaseType = pElementType;
					while (pElementBaseType->GetBaseType())
						pElementBaseType = pElementBaseType->GetBaseType();

					// for now only support pointers to Object.
					assert(pElementBaseType->GetSid() == CONSTSID("Systems::Object"));

					//now let the user choose what class to instanciate.
					ClassSelectionDialog* pDialog = new ClassSelectionDialog(pElementType->GetSid());
					pDialog->Open();
					pDialog->OnOk([this, pMemberPtr](const Core::Sid& classNameSid) 
						{
							const Systems::TypeDescriptor* pTypeToCreate = Systems::ReflectionMgr::Get().GetType(classNameSid);
							if (!pTypeToCreate)
								return;

							void* pNewItem = nullptr;

							if (pTypeToCreate->IsGameComponent())
								pNewItem = Systems::CreateNewGameComponent(pTypeToCreate);
							else if (pTypeToCreate->IsGameObject())
								pNewItem = Systems::CreateNewGameObject(pTypeToCreate);
							else if (pTypeToCreate->IsObject())
								pNewItem = Systems::CreateObject(pTypeToCreate);
							else
								assert(false); // we can only create Object derived class here

							if (!pNewItem)
								return;

							Core::BaseArray* pArray = static_cast<Core::BaseArray*>(pMemberPtr);
							pArray->AddElement();
							void* ppItem = pArray->GetElement(pArray->GetSize()-1); //ppItem is actually a Object**
							uint64_t* pTemp = reinterpret_cast<uint64_t*>(ppItem);
							*pTemp = reinterpret_cast<uint64_t>(pNewItem);

							m_onDataChanged();
						});
				}
				else
				{
					//it's not a pointer so the array will take care or allocating the object
					Core::BaseArray* pArray = static_cast<Core::BaseArray*>(pMemberPtr);
					pArray->AddElement();
					m_onDataChanged();
				}
				
			});

		PropertyGridItem* pItem = new PropertyGridItem(nullptr, pAddElementButton);
		m_pPropertyGridWidget->AddProperty(pItem);
	}

	PropertyGridItem* PropertyGridPopulator::CreatePropertyItemForPODField(const Systems::FieldDescriptor* pField, void* pObj, uint32_t indexElement)
	{
		Widgets::Widget* pEditingWidget = nullptr;

		const Systems::TypeDescriptor* pFieldType = pField->GetType()->IsContainer() ? pField->GetType()->GetElementType() : pField->GetType();
		void* pRawValue = nullptr;
		
		bool isArray = pField->GetType()->IsContainer();
		if (isArray)
		{
			Core::BaseArray* pArray = pField->GetDataPtr<Core::BaseArray>(pObj);
			pRawValue = pArray->GetElement(indexElement);
		}
		else
		{
			pRawValue = pField->GetDataPtr(pObj);
		}

		bool readOnly = pField->IsReadOnly();
		ObjectWatcher::OPERATION op = isArray ? ObjectWatcher::SET_ELEMENT : ObjectWatcher::SET_FIELD;

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
			break;
		}

		PropertyGridItem* pItem = nullptr;
		if (pField->GetType()->IsContainer())
		{
			Core::BaseArray* pArray = pField->GetDataPtr<Core::BaseArray>(pObj);
			bool elementIsObject = pField->GetType()->GetElementType()->IsObject();
			void* pElement = pArray->GetElement(indexElement);
			Widgets::Widget* pNameWidget = CreateArrayItemName(pArray, indexElement, elementIsObject, pElement);
			pItem = new PropertyGridItem(pNameWidget, pEditingWidget);
		}
		else
		{
			pItem = new PropertyGridItem(pField->GetName(), pEditingWidget);
		}

		return pItem;
	}

	PropertyGridItemFactory* PropertyGridPopulator::GetFactory(Core::Sid typeSid) const
	{
		std::map<Core::Sid, PropertyGridItemFactory*>::const_iterator it = m_factories.find(typeSid);
		if (it != m_factories.cend())
			return it->second;

		return nullptr;
	}

	Widgets::Widget* PropertyGridPopulator::CreateArrayItemName(Core::BaseArray* pArray, int elementIndex, bool elementIsObject, void* pElement)
	{
		Widgets::Layout* pNameLayout = new Widgets::Layout(Widgets::Layout::Horizontal, Widgets::Widget::FIT);
		pNameLayout->GetDefaultStyle().SetBackgroundColor(Widgets::Color(0, 0, 0, 0));
		pNameLayout->GetHoverStyle().SetBackgroundColor(Widgets::Color(0, 0, 0, 0));
		pNameLayout->SetSpace(DirectX::XMINT2(5, 0));

		const Widgets::WidgetMgr& widgetMgr = Widgets::WidgetMgr::Get();
		Rendering::TextureId deleteTextureId = widgetMgr.GetIconTextureId(Widgets::IconId::kIconClose);
		Widgets::Icon* pDeleteIcon = new Widgets::Icon(deleteTextureId);

		Widgets::Button* pDeleteButton = new Widgets::Button(12, 12, 0, 0);
		pDeleteButton->AddWidget(pDeleteIcon);
		pDeleteButton->SetPositionStyle(Widgets::Widget::HPOSITION_STYLE::NONE, Widgets::Widget::VPOSITION_STYLE::MIDDLE);
		pDeleteButton->OnClick([pArray, elementIndex, this]()
			{
				Editors::OkCancelDialog* pDialog = new Editors::OkCancelDialog("Delete element", "Are you sure you want to delete this element from the array ?");
				pDialog->OnOk([pArray, elementIndex, this]()
					{
						pArray->RemoveElement(elementIndex);
						m_onDataChanged();
					});

				pDialog->Open();
			});

		pNameLayout->AddWidget(pDeleteButton);

		const int BUFFER_SIZE = 64;
		char buffer[BUFFER_SIZE] = { '\0' };

		if (elementIsObject)
		{
			Systems::Object* pObject = reinterpret_cast<Systems::Object*>(pElement);
			const Systems::TypeDescriptor* pObjectType = pObject->GetTypeDescriptor();
			sprintf_s(buffer, "[%d] %s", elementIndex, pObjectType->GetName().c_str());
		}
		else
		{
			sprintf_s(buffer, "[%d]", elementIndex);
		}

		Widgets::Label* pNameLabel = new Widgets::Label(buffer);
		pNameLabel->SetSizeStyle(Widgets::Widget::HSIZE_FIT | Widgets::Widget::VSIZE_DEFAULT);
		pNameLayout->AddWidget(pNameLabel);

		return pNameLayout;
	}

	void PropertyGridPopulator::ObjectWatcherCallback(Systems::Object* pObj, const Systems::FieldDescriptor* pField, ObjectWatcher::OPERATION op, uint32_t index)
	{
		Core::Array<PropertyGridItem*> items = m_pPropertyGridWidget->GetPropertyGridItems();
		Core::Array<PropertyGridItem*>::Iterator it = std::find_if(items.begin(), items.end(), [pObj, pField, index](PropertyGridItem* item) 
			{ return item->IsField(pObj, pField, index); });

		if (it == items.end())
			return;

		switch (op)
		{
		case ObjectWatcher::SET_FIELD:
		case ObjectWatcher::SET_ELEMENT:
			(*it)->UpdateValue();
			break;

		case ObjectWatcher::REMOVE_ELEMENT:
			m_pPropertyGridWidget->RemoveProperty(*it);
			break;
		}

	}
}
