/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/Widgets/PropertyGrid/Items/FloatItem.h"

#include "Editors/ObjectWatcher/ObjectWatcher.h"

#include "Systems/Reflection/FieldDescriptor.h"

#include "Widgets/TextBox.h"

namespace Editors
{
	FloatItem::FloatItem(Systems::Object* pObj, const Systems::FieldDescriptor* pField, uint32_t index)
		: PropertyGridItem(pObj, pField, index)
		, m_pTextbox(nullptr)
	{ }

	FloatItem::~FloatItem()
	{ }

	void FloatItem::CreateWidgets()
	{
		m_pTextbox = new Widgets::TextBox();

		UpdateValue();
		m_pTextbox->SetReadOnly(m_pField->IsReadOnly());

		m_pTextbox->OnValidate([this](const std::string& value)
			{
				ObjectWatcher::OPERATION op = m_pField->GetType()->IsContainer() ? ObjectWatcher::SET_ELEMENT : ObjectWatcher::SET_FIELD;
				char* pEnd = nullptr;
				float newValue = std::strtof(value.c_str(), &pEnd);

				float* pValue = m_pField->GetDataPtr<float>(m_pObj);
				if (*pValue == newValue)
					return;

				ObjectWatcher::Get().ModifyField(m_pObj, m_pField, op, m_index, &newValue);
			});

		m_pEditingWidget = m_pTextbox;

		m_pNameWidget = CreateDefaultItemLabel();
	}

	void FloatItem::UpdateValue()
	{
		const float* pValue = GetDataPtr<const float>();
		const int BUFFER_SIZE = 64;
		char buffer[BUFFER_SIZE] = { '\0' };
		sprintf_s(buffer, BUFFER_SIZE, "%g", *pValue);
		m_pTextbox->SetText(buffer);
	}
}
