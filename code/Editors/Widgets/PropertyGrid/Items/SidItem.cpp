/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/Widgets/PropertyGrid/Items/SidItem.h"

#include "Editors/ObjectWatcher/ObjectWatcher.h"

#include "Systems/Reflection/FieldDescriptor.h"

#include "Widgets/TextBox.h"

namespace Editors
{
	SidItem::SidItem(Systems::Object* pObj, const Systems::FieldDescriptor* pField, uint32_t index)
		: PropertyGridItem(pObj, pField, index)
		, m_pTextbox(nullptr)
	{ }

	SidItem::~SidItem()
	{ }

	void SidItem::CreateWidgets()
	{
		m_pTextbox = new Widgets::TextBox();

		UpdateValue();
		m_pTextbox->SetReadOnly(m_pField->IsReadOnly());

		m_pTextbox->OnValidate([this](const std::string& value)
			{
				ObjectWatcher::OPERATION op = m_pField->IsContainer() ? ObjectWatcher::SET_ELEMENT : ObjectWatcher::SET_FIELD;
				Core::Sid newSid = Core::MakeSid(value);
				ObjectWatcher::Get().ModifyField(m_pObj, m_pField, op, m_index, &newSid);
			});

		m_pEditingWidget = m_pTextbox;

		m_pNameWidget = CreateDefaultItemLabel();
	}

	void SidItem::UpdateValue()
	{
		const Core::Sid* pValue = GetDataPtr<const Core::Sid>();
		std::string strSid = Core::ToString(*pValue);
		m_pTextbox->SetText(strSid);
	}
}
