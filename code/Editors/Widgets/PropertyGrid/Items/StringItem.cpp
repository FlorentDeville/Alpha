/********************************************************************************/
/* Copyright (C) 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Editors/Widgets/PropertyGrid/Items/StringItem.h"

#include "Core/Reflection/FieldDescriptor.h"

#include "Editors/ObjectWatcher/ObjectWatcher.h"

#include "Widgets/TextBox.h"

namespace Editors
{
	StringItem::StringItem(void* pObj, const Core::FieldDescriptor* pField, uint32_t index)
		: PropertyGridItem(pObj, pField, index)
		, m_pTextbox(nullptr)
	{ }

	StringItem::~StringItem()
	{ }

	void StringItem::CreateWidgets()
	{
		m_pNameWidget = PropertyGridItem::CreateDefaultItemLabel();


		m_pTextbox = new Widgets::TextBox();

		UpdateValue();
		m_pTextbox->SetReadOnly(m_pField->IsReadOnly());

		m_pTextbox->OnValidate([this](const std::string& value)
			{
				std::string* pValue = m_pField->GetDataPtr<std::string>(m_pObj);
				if (value == *pValue)
					return;

				m_onBeforeValueUpdated(pValue);

				ObjectWatcher::OPERATION op = m_pField->GetType()->IsContainer() ? ObjectWatcher::SET_ELEMENT : ObjectWatcher::SET_FIELD;
				ObjectWatcher::Get().ModifyField(m_pObj, m_pField, op, m_index, &value);

				m_onAfterValueUpdated(pValue);
			});

		m_pEditingWidget = m_pTextbox;
	}

	void StringItem::UpdateValue()
	{
		const std::string* pValue = GetDataPtr<const std::string>();
		m_pTextbox->SetText(*pValue);
	}
}
