/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/Widgets/PropertyGrid/Items/GuidItem.h"

#include "Core/Guid/Guid.h"

#include "Editors/ObjectWatcher/ObjectWatcher.h"

#include "Systems/Reflection/FieldDescriptor.h"

#include "Widgets/TextBox.h"

namespace Editors
{
	GuidItem::GuidItem(Systems::Object* pObj, const Systems::FieldDescriptor* pField, uint32_t index)
		: PropertyGridItem(pObj, pField, index)
		, m_pTextbox(nullptr)
	{ }

	GuidItem::~GuidItem()
	{ }

	void GuidItem::CreateWidgets()
	{
		m_pTextbox = new Widgets::TextBox();

		UpdateValue();

		m_pTextbox->SetReadOnly(m_pField->IsReadOnly());

		m_pTextbox->OnValidate([this](const std::string& value)
			{
				ObjectWatcher::OPERATION op = m_pField->IsContainer() ? ObjectWatcher::SET_ELEMENT : ObjectWatcher::SET_FIELD;
				Core::Guid newGuid(value.c_str());
				ObjectWatcher::Get().ModifyField(m_pObj, m_pField, op, m_index, &newGuid);
			});

		m_pEditingWidget = m_pTextbox;

		m_pNameWidget = CreateDefaultItemLabel();
	}

	void GuidItem::UpdateValue()
	{
		const Core::Guid* pValue = GetDataPtr<const Core::Guid>();

		const int BUFFER_SIZE = 64;
		char buffer[BUFFER_SIZE] = { '\0' };
		pValue->ToString(buffer, BUFFER_SIZE);

		m_pTextbox->SetText(buffer);
	}
}
