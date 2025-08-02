/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/Widgets/PropertyGrid/Items/StringItem.h"

#include "Editors/ObjectWatcher/ObjectWatcher.h"

#include "Systems/Reflection/FieldDescriptor.h"

#include "Widgets/TextBox.h"

namespace Editors
{
	StringItem::StringItem(Systems::Object* pObj, Systems::FieldDescriptor* pField, uint32_t index)
		: PropertyGridItem(pObj, pField, index)
	{ }

	StringItem::~StringItem()
	{ }

	void StringItem::CreateWidgets()
	{
		m_pNameWidget = PropertyGridItem::CreateDefaultItemLabel();


		m_pTextbox = new Widgets::TextBox();

		std::string* pValue = m_pField->GetDataPtr<std::string>(m_pObj);
		m_pTextbox->SetText(*pValue);
		m_pTextbox->SetReadOnly(m_pField->IsReadOnly());

		m_pTextbox->OnValidate([this](const std::string& value)
			{
				std::string* pValue = m_pField->GetDataPtr<std::string>(m_pObj);
				if (value == *pValue)
					return;

				ObjectWatcher::OPERATION op = m_pField->IsContainer() ? ObjectWatcher::SET_ELEMENT : ObjectWatcher::SET_FIELD;
				ObjectWatcher::Get().ModifyField(m_pObj, m_pField, op, m_index, &value);
			});

		m_pEditingWidget = m_pTextbox;
	}

	void StringItem::UpdateValue()
	{
		std::string* pValue = m_pField->GetDataPtr<std::string>(m_pObj);
		m_pTextbox->SetText(*pValue);
	}
}
