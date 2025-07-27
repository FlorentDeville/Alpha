/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/Widgets/PropertyGrid/PropertyGridPopulator.h"

#include "Core/Guid/Guid.h"
#include "Core/Math/Mat44f.h"
#include "Core/Math/Vec4f.h"

#include "Editors/Widgets/Dialog/ClassSelectionDialog.h"
#include "Editors/Widgets/Dialog/OkCancelDialog.h"
#include "Editors/Widgets/PropertyGrid/PropertyGridItem.h"
#include "Editors/Widgets/PropertyGrid/PropertyGridItemFactory.h"
#include "Editors/Widgets/PropertyGrid/PropertyGridWidget.h"

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
	{ }

	PropertyGridPopulator::~PropertyGridPopulator()
	{
		for (const std::pair<Core::Sid, PropertyGridItemFactory*>& pair : m_factories)
		{
			delete pair.second;
		}

		m_factories.clear();
	}

	void PropertyGridPopulator::Init(PropertyGridWidget* pWidget)
	{
		m_pPropertyGridWidget = pWidget;
	}

	void PropertyGridPopulator::Populate(Systems::Object* pObject)
	{
		m_pObject = pObject;
		
		CreatePropertiesForObject(pObject, 0);

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

	void PropertyGridPopulator::CreatePropertiesForArrayElements(const Systems::FieldDescriptor* pField, void* pArrayPtr, int depth)
	{
		Core::BaseArray* pArray = reinterpret_cast<Core::BaseArray*>(pArrayPtr);

		int32_t size = pArray->GetSize();

		const Systems::TypeDescriptor* pElementType = pField->GetElementType();
		bool isObject = pElementType->IsObject();

		for (int ii = 0; ii < size; ++ii)
		{
			void* pElement = pArray->GetElement(ii);
			if (pField->IsElementPointer())
			{
				uint64_t* pCharPtr = reinterpret_cast<uint64_t*>(pElement);
				pElement = reinterpret_cast<uint64_t*>(*pCharPtr);
			}

			Widgets::Layout* pNameLayout = nullptr;
			{
				pNameLayout = new Widgets::Layout(Widgets::Layout::Horizontal, Widgets::Widget::FIT);
				pNameLayout->GetDefaultStyle().SetBackgroundColor(Widgets::Color(0, 0, 0, 0));
				pNameLayout->GetHoverStyle().SetBackgroundColor(Widgets::Color(0, 0, 0, 0));
				pNameLayout->SetSpace(DirectX::XMINT2(5, 0));

				const Widgets::WidgetMgr& widgetMgr = Widgets::WidgetMgr::Get();
				Rendering::TextureId deleteTextureId = widgetMgr.GetIconTextureId(Widgets::IconId::kIconClose);
				Widgets::Icon* pDeleteIcon = new Widgets::Icon(deleteTextureId);

				Widgets::Button* pDeleteButton = new Widgets::Button(12, 12, 0, 0);
				pDeleteButton->AddWidget(pDeleteIcon);
				pDeleteButton->SetPositionStyle(Widgets::Widget::HPOSITION_STYLE::NONE, Widgets::Widget::VPOSITION_STYLE::MIDDLE);
				pDeleteButton->OnClick([pArray, ii, this]()
					{
						Editors::OkCancelDialog* pDialog = new Editors::OkCancelDialog("Delete element", "Are you sure you want to delete this element from the array ?");
						pDialog->OnOk([pArray, ii, this]()
							{
								pArray->RemoveElement(ii);
								m_onDataChanged();
							});

						pDialog->Open();
					});

				pNameLayout->AddWidget(pDeleteButton);

				const int BUFFER_SIZE = 64;
				char buffer[BUFFER_SIZE] = { '\0' };

				if (isObject)
				{
					Systems::Object* pObject = reinterpret_cast<Systems::Object*>(pElement);
					const Systems::TypeDescriptor* pObjectType = pObject->GetTypeDescriptor();
					sprintf_s(buffer, "[%d] %s", ii, pObjectType->GetName().c_str());
				}
				else
				{
					sprintf_s(buffer, "[%d]", ii);
				}

				Widgets::Label* pNameLabel = new Widgets::Label(buffer);
				pNameLabel->SetSizeStyle(Widgets::Widget::HSIZE_FIT | Widgets::Widget::VSIZE_DEFAULT);
				pNameLayout->AddWidget(pNameLabel);			
			}

			if (PropertyGridItemFactory* pFactory = GetFactory(pElementType->GetSid()))
			{
				pFactory->CreateItems(pElementType, pElement, depth + 1);
			}
			else if (pElementType->GetSid() == CONSTSID("Core::Array"))
			{
				assert(false); //don't support array of arrays for now
			}
			else if (isObject)
			{
				PropertyGridItem* pItem = new PropertyGridItem(pNameLayout, nullptr);
				m_pPropertyGridWidget->AddProperty(pItem, depth);

				Systems::Object* pObject = reinterpret_cast<Systems::Object*>(pElement);
				CreatePropertiesForObject(pObject, depth + 1);
			}
			else if (pElementType->IsClass())
			{
				PropertyGridItem* pItem = new PropertyGridItem(pNameLayout, nullptr);
				m_pPropertyGridWidget->AddProperty(pItem, depth);

				CreatePropertiesForTypeMembers(pElementType, pElement, depth + 1);
			}
			else //pod
			{
				Widgets::Widget* pWidget = CreateWidgetForPODField(pElementType, pElement, pField->IsReadOnly());

				
				PropertyGridItem* pItem = new PropertyGridItem(pNameLayout, pWidget);
				m_pPropertyGridWidget->AddProperty(pItem, depth);
			}
		}
	}

	void PropertyGridPopulator::CreatePropertiesForTypeMembers(const Systems::TypeDescriptor* pFieldType, void* pData, int depth)
	{
		const std::vector<Systems::FieldDescriptor>& members = pFieldType->GetFields();
		for (const Systems::FieldDescriptor& member : members)
		{
			if (member.IsHidden())
				continue;

			const Systems::TypeDescriptor* memberType = member.GetType();

			void* pMemberPtr = member.GetDataPtr(pData);
			if (member.IsPointer())
			{
				char* pCharPtr = reinterpret_cast<char*>(pMemberPtr);
				pMemberPtr = reinterpret_cast<char*>(*pCharPtr);
			}

			if (memberType->GetSid() == CONSTSID("Core::Array"))
			{
				PropertyGridItem* pItem = new PropertyGridItem(member.GetName(), nullptr);
				m_pPropertyGridWidget->AddProperty(pItem, depth);

				CreatePropertiesForArrayElements(&member, pMemberPtr, depth + 1);
				
				if(m_canAddElementToArray)
					CreateArrayAddElementButton(member, pMemberPtr);
			}
			else if (memberType->IsObject())
			{
				Systems::Object* pObject = reinterpret_cast<Systems::Object*>(pMemberPtr);
				CreatePropertiesForObject(pObject, depth + 1);
			}
			else if (memberType->IsClass())
			{
				PropertyGridItem* pItem = new PropertyGridItem(member.GetName(), nullptr);
				m_pPropertyGridWidget->AddProperty(pItem);

				CreatePropertiesForTypeMembers(memberType, pMemberPtr, depth + 1);
			}
			else //pod
			{
				Widgets::Widget* pWidget = CreateWidgetForPODField(memberType, pMemberPtr, member.IsReadOnly());

				PropertyGridItem* pItem = new PropertyGridItem(member.GetName(), pWidget);
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
				if (member.IsElementPointer())
				{
					const Systems::TypeDescriptor* pElementType = member.GetElementType();

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

	Widgets::Widget* PropertyGridPopulator::CreateWidgetForPODField(const Systems::TypeDescriptor* pFieldType, void* pData, bool readOnly)
	{
		Widgets::Widget* pEditingWidget = nullptr;

		switch (pFieldType->GetSid())
		{
		case SID("std::string"):
		{
			Widgets::TextBox* pTextBox = new Widgets::TextBox();

			std::string* pValue = reinterpret_cast<std::string*>(pData);
			pTextBox->SetText(*pValue);
			pTextBox->SetReadOnly(readOnly);

			pTextBox->OnValidate([this, pValue](const std::string& value)
				{
					*pValue = value;
					m_onDataChanged();
				});

			pEditingWidget = pTextBox;
		}
		break;

		case SID("uint32_t"):
		{
			Widgets::TextBox* pTextBox = new Widgets::TextBox();

			uint32_t* pValue = reinterpret_cast<uint32_t*>(pData);
			
			const int BUFFER_SIZE = 64;
			char buffer[BUFFER_SIZE] = { '\0' };
			sprintf_s(buffer, BUFFER_SIZE, "%d", *pValue);
			pTextBox->SetText(buffer);
			pTextBox->SetReadOnly(readOnly);

			pTextBox->OnValidate([this, pValue](const std::string& value)
				{
					char* pEnd = nullptr;
					*pValue = std::strtoul(value.c_str(), &pEnd, 10);
					m_onDataChanged();
				});

			pEditingWidget = pTextBox;
		}
		break;

		case SID("Core::Sid"):
		{
			Widgets::TextBox* pTextBox = new Widgets::TextBox();

			Core::Sid* pValue = reinterpret_cast<Core::Sid*>(pData);

			std::string strSid = Core::ToString(*pValue);
			pTextBox->SetText(strSid);
			pTextBox->SetReadOnly(readOnly);

			pTextBox->OnValidate([this, pValue](const std::string& value)
				{
					*pValue = Core::MakeSid(value);
					m_onDataChanged();
				});

			pEditingWidget = pTextBox;
		}
		break;

		case SID("Core::Guid"):
		{
			Widgets::TextBox* pTextBox = new Widgets::TextBox();

			Core::Guid* pValue = reinterpret_cast<Core::Guid*>(pData);

			const int BUFFER_SIZE = 64;
			char buffer[BUFFER_SIZE] = { '\0' };
			pValue->ToString(buffer, BUFFER_SIZE);

			pTextBox->SetText(buffer);
			pTextBox->SetReadOnly(readOnly);

			pTextBox->OnValidate([this, pValue](const std::string& value)
				{
					*pValue = Core::Guid(value.c_str());
					m_onDataChanged();
				});

			pEditingWidget = pTextBox;
		}
		break;

		case SID("Core::Mat44f"):
		{
			Core::Mat44f* pValue = reinterpret_cast<Core::Mat44f*>(pData);

			Widgets::Layout* pLayout = new Widgets::Layout();
			pLayout->SetDirection(Widgets::Layout::Direction::Vertical);
			pLayout->SetSizeStyle(Widgets::Widget::HSIZE_DEFAULT | Widgets::Widget::VSIZE_FIT);
			pLayout->GetDefaultStyle().SetBorderSize(1);
			pLayout->GetDefaultStyle().SetBorderColor(Widgets::Color(255, 0, 0, 255));
			pLayout->GetDefaultStyle().ShowBorder(true);

			for (int row = 0; row < 4; ++row)
			{
				Widgets::Layout* pRowLayout = new Widgets::Layout(Widgets::Layout::Direction::Horizontal, Widgets::Widget::FIT);
				pLayout->AddWidget(pRowLayout);
				for (int column = 0; column < 4; ++column)
				{
					Widgets::TextBox* pTextBox = new Widgets::TextBox();
					float fValue = pValue->Get(row, column);

					const int BUFFER_SIZE = 16;
					char buffer[BUFFER_SIZE] = { '\0' };
					snprintf(buffer, BUFFER_SIZE, "%g", fValue); //%g removes the meaningless 0.

					pTextBox->SetText(buffer);
					pTextBox->SetSizeStyle(Widgets::Widget::DEFAULT);
					pTextBox->SetReadOnly(readOnly);
					pTextBox->SetWidth(40);
					pTextBox->OnValidate([row, column, pValue](const std::string& value)
						{
							float newValue = 0;
							std::from_chars(value.c_str(), value.c_str() + value.size(), newValue);
							pValue->Set(row, column, newValue);
						});
					pRowLayout->AddWidget(pTextBox);
				}
			}

			pEditingWidget = pLayout;
		}
		break;

		default:
			break;
		}

		return pEditingWidget;
	}

	PropertyGridItemFactory* PropertyGridPopulator::GetFactory(Core::Sid typeSid) const
	{
		std::map<Core::Sid, PropertyGridItemFactory*>::const_iterator it = m_factories.find(typeSid);
		if (it != m_factories.cend())
			return it->second;

		return nullptr;
	}
}
