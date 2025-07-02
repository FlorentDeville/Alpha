/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/Widgets/PropertyGrid/PropertyGridPopulator.h"

#include "Editors/Widgets/PropertyGrid/PropertyGridItem.h"
#include "Editors/Widgets/PropertyGrid/PropertyGridWidget.h"

#include "Systems/Objects/Object.h"

#include "Widgets/Label.h"
#include "Widgets/TextBox.h"
#include "Widgets/WidgetMgr.h"

namespace Editors
{
	PropertyGridPopulator::PropertyGridPopulator()
		: m_pPropertyGridWidget(nullptr)
	{ }

	PropertyGridPopulator::~PropertyGridPopulator()
	{ }

	void PropertyGridPopulator::Populate(PropertyGridWidget* pWidget, Systems::Object* pObject)
	{
		m_pPropertyGridWidget = pWidget;

		const Systems::TypeDescriptor* pType = pObject->GetTypeDescriptor();

		CreatePropertiesForClassMember(pType, pObject);

		Widgets::WidgetMgr::Get().RequestResize();
	}

	void PropertyGridPopulator::CreatePropertiesForArrayElements(const Systems::FieldDescriptor* pField, void* pArrayPtr)
	{
		Core::BaseArray* pArray = reinterpret_cast<Core::BaseArray*>(pArrayPtr);

		int32_t size = pArray->GetSize();

		const Systems::TypeDescriptor* pElementType = pField->GetElementType();

		for (int ii = 0; ii < size; ++ii)
		{
			void* pElement = pArray->GetElement(ii);
			if (pField->IsElementPointer())
			{
				char* pCharPtr = reinterpret_cast<char*>(pElement);
				pElement = reinterpret_cast<char*>(*pCharPtr);
			}

			const int BUFFER_SIZE = 8;
			char buffer[BUFFER_SIZE] = { '\0' };
			_itoa_s(ii, buffer, BUFFER_SIZE, 10);

			if (pElementType->GetSid() == CONSTSID("Core::Array"))
			{
				assert(false); //don't support array of arrays for now
			}
			else if (pElementType->IsClass())
			{
				PropertyGridItem* pItem = new PropertyGridItem(buffer, nullptr);
				m_pPropertyGridWidget->AddProperty(pItem);

				CreatePropertiesForClassMember(pElementType, pElement);
			}
			else //pod
			{
				Widgets::Widget* pWidget = CreateWidgetForPODField(pElementType, pElement);

				
				PropertyGridItem* pItem = new PropertyGridItem(buffer, pWidget);
				m_pPropertyGridWidget->AddProperty(pItem);
			}
		}
	}

	void PropertyGridPopulator::CreatePropertiesForClassMember(const Systems::TypeDescriptor* pFieldType, void* pData)
	{
		const std::vector<Systems::FieldDescriptor>& members = pFieldType->GetFields();
		for (const Systems::FieldDescriptor& member : members)
		{
			if (!member.IsEditable())
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
				m_pPropertyGridWidget->AddProperty(pItem);

				CreatePropertiesForArrayElements(&member, pMemberPtr);
			}
			else if (memberType->IsClass())
			{
				PropertyGridItem* pItem = new PropertyGridItem(member.GetName(), nullptr);
				m_pPropertyGridWidget->AddProperty(pItem);

				CreatePropertiesForClassMember(memberType, pMemberPtr);
			}
			else //pod
			{
				Widgets::Widget* pWidget = CreateWidgetForPODField(memberType, pMemberPtr);

				PropertyGridItem* pItem = new PropertyGridItem(member.GetName(), pWidget);
				m_pPropertyGridWidget->AddProperty(pItem);
			}
		}

	}

	Widgets::Widget* PropertyGridPopulator::CreateWidgetForPODField(const Systems::TypeDescriptor* pFieldType, void* pData)
	{
		Widgets::Widget* pEditingWidget = nullptr;

		switch (pFieldType->GetSid())
		{
		case SID("std::string"):
		{
			Widgets::TextBox* pTextBox = new Widgets::TextBox();

			std::string* pValue = reinterpret_cast<std::string*>(pData);
			pTextBox->SetText(*pValue);
			pEditingWidget = pTextBox;

			pTextBox->OnValidate([this, pValue](const std::string& value)
				{
					*pValue = value;
					m_onDataChanged();
				});

			pEditingWidget = pTextBox;
		}
		break;

		default:
			break;
		}

		return pEditingWidget;
	}
}
