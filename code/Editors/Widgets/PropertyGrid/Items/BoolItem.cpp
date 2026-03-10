/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Editors/Widgets/PropertyGrid/Items/BoolItem.h"

#include "Core/Reflection/FieldDescriptor.h"

#include "Editors/ObjectWatcher/ObjectWatcher.h"

#include "Widgets/Widgets/CheckBox.h"

namespace Editors
{
	BoolItem::BoolItem(Systems::Object* pObj, const Core::FieldDescriptor* pField, uint32_t index)
		: PropertyGridItem(pObj, pField, index)
		, m_pCheckbox(nullptr)
	{ }

	BoolItem::~BoolItem()
	{ }

	void BoolItem::CreateWidgets()
	{
		m_pCheckbox = new Widgets::CheckBox(false);

		UpdateValue();
		//m_pTextbox->SetReadOnly(m_pField->IsReadOnly());

		m_pCheckbox->OnValueChanged([this](const bool newValue)
			{
				ObjectWatcher::OPERATION op = m_pField->GetType()->IsContainer() ? ObjectWatcher::SET_ELEMENT : ObjectWatcher::SET_FIELD;

				bool* pValue = m_pField->GetDataPtr<bool>(m_pObj);
				if (*pValue == newValue)
					return;

				ObjectWatcher::Get().ModifyField(m_pObj, m_pField, op, m_index, &newValue);
			});

		m_pEditingWidget = m_pCheckbox;

		m_pNameWidget = CreateDefaultItemLabel();
	}

	void BoolItem::UpdateValue()
	{
		const bool* pValue = GetDataPtr<const bool>();
		m_pCheckbox->SetValue(*pValue);
	}
}
