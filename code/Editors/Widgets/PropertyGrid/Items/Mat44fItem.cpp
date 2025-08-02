/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/Widgets/PropertyGrid/Items/Mat44fItem.h"

#include "Core/Math/Mat44f.h"

#include "Editors/ObjectWatcher/ObjectWatcher.h"

#include "Systems/Reflection/FieldDescriptor.h"

#include "Widgets/Layout.h"
#include "Widgets/TextBox.h"

#include <charconv>

namespace Editors
{
	Mat44fItem::Mat44fItem(Systems::Object* pObj, const Systems::FieldDescriptor* pField, uint32_t index)
		: PropertyGridItem(pObj, pField, index)
		, m_pTextbox()
	{ }

	Mat44fItem::~Mat44fItem()
	{ }

	void Mat44fItem::CreateWidgets()
	{
		Widgets::Layout* pLayout = new Widgets::Layout();
		pLayout->SetDirection(Widgets::Layout::Direction::Vertical);
		pLayout->SetSizeStyle(Widgets::Widget::HSIZE_DEFAULT | Widgets::Widget::VSIZE_FIT);
		pLayout->GetDefaultStyle().SetBorderSize(1);
		pLayout->GetDefaultStyle().SetBorderColor(Widgets::Color(255, 0, 0, 255));
		pLayout->GetDefaultStyle().ShowBorder(true);

		for (int row = 0; row < 4; ++row)
		{
			Widgets::Layout* pRowLayout = new Widgets::Layout(Widgets::Layout::Direction::Horizontal, Widgets::Widget::FIT);
			pLayout->AddWidget(pRowLayout);
			for (int column = 0; column < 4; ++column)
			{
				m_pTextbox[row][column] = new Widgets::TextBox();
				m_pTextbox[row][column]->SetSizeStyle(Widgets::Widget::DEFAULT);
				m_pTextbox[row][column]->SetReadOnly(m_pField->IsReadOnly());
				m_pTextbox[row][column]->SetWidth(40);
				m_pTextbox[row][column]->OnValidate([row, column, this](const std::string& value)
					{
						ObjectWatcher::OPERATION op = m_pField->IsContainer() ? ObjectWatcher::SET_ELEMENT : ObjectWatcher::SET_FIELD;

						float newValue = 0;
						std::from_chars(value.c_str(), value.c_str() + value.size(), newValue);

						const Core::Mat44f* pValue = GetDataPtr<const Core::Mat44f>();
						Core::Mat44f copy = *pValue;
						copy.Set(row, column, newValue);
						ObjectWatcher::Get().ModifyField(m_pObj, m_pField, op, m_index, &copy);
					});
				pRowLayout->AddWidget(m_pTextbox[row][column]);
			}
		}
		
		UpdateValue();

		m_pNameWidget = CreateDefaultItemLabel();
		m_pEditingWidget = pLayout;
	}

	void Mat44fItem::UpdateValue()
	{
		const Core::Mat44f* pValue = GetDataPtr<const Core::Mat44f>();

		for (int row = 0; row < 4; ++row)
		{
			for (int column = 0; column < 4; ++column)
			{
				float fValue = pValue->Get(row, column);

				const int BUFFER_SIZE = 16;
				char buffer[BUFFER_SIZE] = { '\0' };
				snprintf(buffer, BUFFER_SIZE, "%g", fValue); //%g removes the meaningless 0.

				m_pTextbox[row][column]->SetText(buffer);
			}
		}
	}
}
