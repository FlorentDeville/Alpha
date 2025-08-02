/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/Widgets/PropertyGrid/Items/UInt32Item.h"

#include "Editors/ObjectWatcher/ObjectWatcher.h"

#include "Systems/Reflection/FieldDescriptor.h"

#include "Widgets/TextBox.h"

namespace Editors
{
	UInt32Item::UInt32Item(Systems::Object* pObj, const Systems::FieldDescriptor* pField, uint32_t index)
		: PropertyGridItem(pObj, pField, index)
		, m_pTextbox(nullptr)
	{ }

	UInt32Item::~UInt32Item()
	{ }

	void UInt32Item::CreateWidgets()
	{
		m_pTextbox = new Widgets::TextBox();

		uint32_t* pValue = m_pField->GetDataPtr<uint32_t>(m_pObj);

		const int BUFFER_SIZE = 64;
		char buffer[BUFFER_SIZE] = { '\0' };
		sprintf_s(buffer, BUFFER_SIZE, "%d", *pValue);
		m_pTextbox->SetText(buffer);
		m_pTextbox->SetReadOnly(m_pField->IsReadOnly());

		m_pTextbox->OnValidate([this](const std::string& value)
			{
				ObjectWatcher::OPERATION op = m_pField->IsContainer() ? ObjectWatcher::SET_ELEMENT : ObjectWatcher::SET_FIELD;
				char* pEnd = nullptr;
				uint32_t newValue = std::strtoul(value.c_str(), &pEnd, 10);

				uint32_t* pValue = m_pField->GetDataPtr<uint32_t>(m_pObj);
				if (*pValue == newValue)
					return;

				ObjectWatcher::Get().ModifyField(m_pObj, m_pField, op, m_index, &newValue);
			});

		m_pEditingWidget = m_pTextbox;

		m_pNameWidget = CreateDefaultItemLabel();
	}

	void UInt32Item::UpdateValue()
	{
		uint32_t* pValue = m_pField->GetDataPtr<uint32_t>(m_pObj);
		const int BUFFER_SIZE = 64;
		char buffer[BUFFER_SIZE] = { '\0' };
		sprintf_s(buffer, BUFFER_SIZE, "%d", *pValue);
		m_pTextbox->SetText(buffer);
	}
}
