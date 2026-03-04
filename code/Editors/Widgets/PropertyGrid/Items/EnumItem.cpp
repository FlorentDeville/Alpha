/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Editors/Widgets/PropertyGrid/Items/EnumItem.h"

#include "Widgets/Widgets/ComboBox.h"

namespace Editors
{
	EnumItem::EnumItem(void* pObj, const Core::FieldDescriptor* pField, uint32_t index)
		: PropertyGridItem(pObj, pField, index)
		, m_pComboBox(nullptr)
	{ }

	EnumItem::~EnumItem()
	{ }

	void EnumItem::CreateWidgets()
	{
		m_pComboBox = new Widgets::ComboBox();

		const Core::TypeDescriptor* pType = m_pField->GetType();
		const Core::Array<Core::EnumEntry>& enumEntries = pType->GetEnumEntries();
		for (const Core::EnumEntry& entry : enumEntries)
			m_pComboBox->AddOption(entry.GetName(), entry.GetValue());

		switch (pType->GetSize())
		{
		case 1: 
		{
			int8_t* pValue = m_pField->GetDataPtr<int8_t>(m_pObj);
			int64_t storedValue = Core::EnumEntry::ToStoredValue(*pValue);
			m_pComboBox->SetSelection(storedValue);
		}
		break;

		case 2:
		{
			int16_t* pValue = m_pField->GetDataPtr<int16_t>(m_pObj);
			int64_t storedValue = Core::EnumEntry::ToStoredValue(*pValue);
			m_pComboBox->SetSelection(storedValue);
		}
		break;

		case 4:
		{
			int32_t* pValue = m_pField->GetDataPtr<int32_t>(m_pObj);
			int64_t storedValue = Core::EnumEntry::ToStoredValue(*pValue);
			m_pComboBox->SetSelection(storedValue);
		}
		break;

		case 8:
		{
			int64_t* pValue = m_pField->GetDataPtr<int64_t>(m_pObj);
			m_pComboBox->SetSelection(*pValue);
		}
		break;
		}

		m_pEditingWidget = m_pComboBox;
		m_pNameWidget = CreateDefaultItemLabel();
	}

	void EnumItem::UpdateValue()
	{ }
}
