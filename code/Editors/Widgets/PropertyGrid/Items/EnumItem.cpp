/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Editors/Widgets/PropertyGrid/Items/EnumItem.h"

#include "Editors/ObjectWatcher/ObjectWatcher.h"

#include "Widgets/Widgets/ComboBox.h"

namespace Editors
{
	EnumItem::EnumItem(void* pObj, const Core::FieldDescriptor* pField, uint32_t index)
		: PropertyGridItem(pObj, pField, index)
		, m_pComboBox(nullptr)
		, m_selectedEnum()
	{ }

	EnumItem::~EnumItem()
	{ }

	void EnumItem::CreateWidgets()
	{
		m_pComboBox = new Widgets::ComboBox();
		m_pComboBox->OnSelectionChanged([this](int64_t oldValue, int64_t newValue) { OnSelectionChanged(oldValue, newValue); });
		m_pComboBox->SetReadOnly(m_pField->IsReadOnly());

		const Core::TypeDescriptor* pType = m_pField->GetType();
		const Core::Array<Core::EnumEntry>& enumEntries = pType->GetEnumEntries();
		for (const Core::EnumEntry& entry : enumEntries)
			m_pComboBox->AddOption(entry.GetName(), entry.GetValue());

		UpdateValue();

		m_pEditingWidget = m_pComboBox;
		m_pNameWidget = CreateDefaultItemLabel();
	}

	void EnumItem::UpdateValue()
	{
		const Core::TypeDescriptor* pType = m_pField->GetType();
		switch (pType->GetSize())
		{
		case 1:
		{
			int8_t* pValue = m_pField->GetDataPtr<int8_t>(m_pObj);
			m_selectedEnum = Core::EnumEntry::ToStoredValue(*pValue);
			m_pComboBox->SetSelection(m_selectedEnum);
		}
		break;

		case 2:
		{
			int16_t* pValue = m_pField->GetDataPtr<int16_t>(m_pObj);
			m_selectedEnum = Core::EnumEntry::ToStoredValue(*pValue);
			m_pComboBox->SetSelection(m_selectedEnum);
		}
		break;

		case 4:
		{
			int32_t* pValue = m_pField->GetDataPtr<int32_t>(m_pObj);
			m_selectedEnum = Core::EnumEntry::ToStoredValue(*pValue);
			m_pComboBox->SetSelection(m_selectedEnum);
		}
		break;

		case 8:
		{
			m_selectedEnum = *m_pField->GetDataPtr<int64_t>(m_pObj);
			m_pComboBox->SetSelection(m_selectedEnum);
		}
		break;
		}
	}

	void EnumItem::OnSelectionChanged(int64_t oldValue, int64_t newValue)
	{
		if (m_selectedEnum == newValue)
			return;

		m_selectedEnum = newValue;

		ObjectWatcher::OPERATION op = m_pField->GetType()->IsContainer() ? ObjectWatcher::SET_ELEMENT : ObjectWatcher::SET_FIELD;
		const Core::TypeDescriptor* pType = m_pField->GetType();

		switch (pType->GetSize())
		{
		case 1:
		{
			int8_t enumValue = Core::EnumEntry::To1Byte(newValue);
			ObjectWatcher::Get().ModifyField(m_pObj, m_pField, op, m_index, &enumValue);
		}
		break;

		case 2:
		{
			int16_t enumValue = Core::EnumEntry::To2Bytes(newValue);
			ObjectWatcher::Get().ModifyField(m_pObj, m_pField, op, m_index, &enumValue);
		}
		break;

		case 4:
		{
			int32_t enumValue = Core::EnumEntry::To4Bytes(newValue);
			ObjectWatcher::Get().ModifyField(m_pObj, m_pField, op, m_index, &enumValue);
		}
		break;

		case 8:
		{
			ObjectWatcher::Get().ModifyField(m_pObj, m_pField, op, m_index, &newValue);
		}
		break;

		default:
			assert(false && "Unsupported enum size");
			break;
		}
	}
}
