/********************************************************************/
/* © 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/Widgets/PropertyGrid/Items/Vec4fItem.h"

#include "Core/Math/Vec4f.h"

#include "Editors/ObjectWatcher/ObjectWatcher.h"

#include "Systems/Reflection/FieldDescriptor.h"

#include "Widgets/Layout.h"
#include "Widgets/TextBox.h"

#include <charconv>

namespace Editors
{
	Vec4fItem::Vec4fItem(Systems::Object* pObj, const Systems::FieldDescriptor* pField, uint32_t index)
		: PropertyGridItem(pObj, pField, index)
		, m_pTextbox()
	{ }

	Vec4fItem::~Vec4fItem()
	{ }

	void Vec4fItem::CreateWidgets()
	{
		Widgets::Layout* pLayout = new Widgets::Layout();
		pLayout->SetDirection(Widgets::Layout::Direction::Horizontal);
		pLayout->SetSizeStyle(Widgets::Widget::HSIZE_DEFAULT | Widgets::Widget::VSIZE_FIT);
		pLayout->GetDefaultStyle().SetBorderSize(1);
		pLayout->GetDefaultStyle().SetBorderColor(Widgets::Color(255, 0, 0, 255));
		pLayout->GetDefaultStyle().ShowBorder(true);

		for (int ii = 0; ii < 4; ++ii)
		{
			m_pTextbox[ii] = new Widgets::TextBox();
			m_pTextbox[ii]->SetSizeStyle(Widgets::Widget::DEFAULT);
			m_pTextbox[ii]->SetReadOnly(m_pField->IsReadOnly());
			m_pTextbox[ii]->SetWidth(40);
			m_pTextbox[ii]->OnValidate([ii, this](const std::string& value)
				{
					ObjectWatcher::OPERATION op = m_pField->GetType()->IsContainer() ? ObjectWatcher::SET_ELEMENT : ObjectWatcher::SET_FIELD;

					float newValue = 0;
					std::from_chars(value.c_str(), value.c_str() + value.size(), newValue);

					const Core::Vec4f* pValue = GetDataPtr<const Core::Vec4f>();
					Core::Vec4f copy = *pValue;
					copy.Set(ii, newValue);
					ObjectWatcher::Get().ModifyField(m_pObj, m_pField, op, m_index, &copy);
				});
			pLayout->AddWidget(m_pTextbox[ii]);
		}

		UpdateValue();

		m_pNameWidget = CreateDefaultItemLabel();
		m_pEditingWidget = pLayout;
	}

	void Vec4fItem::UpdateValue()
	{
		const Core::Vec4f* pValue = GetDataPtr<const Core::Vec4f>();

		for (int ii = 0; ii < 4; ++ii)
		{
			float fValue = pValue->Get(ii);

			const int BUFFER_SIZE = 16;
			char buffer[BUFFER_SIZE] = { '\0' };
			snprintf(buffer, BUFFER_SIZE, "%g", fValue); //%g removes the meaningless 0.

			m_pTextbox[ii]->SetText(buffer);
		}
	}
}
