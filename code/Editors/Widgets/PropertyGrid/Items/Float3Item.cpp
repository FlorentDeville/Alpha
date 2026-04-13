/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Editors/Widgets/PropertyGrid/Items/Float3Item.h"

#include "Core/Math/Vectors.h"
#include "Core/Reflection/FieldDescriptor.h"

#include "Editors/ObjectWatcher/ObjectWatcher.h"

#include "Widgets/Layout.h"
#include "Widgets/TextBox.h"

#include <charconv>

namespace Editors
{
	Float3Item::Float3Item(void* pObj, const Core::FieldDescriptor* pField, uint32_t index)
		: PropertyGridItem(pObj, pField, index)
		, m_pTextbox()
	{ }

	Float3Item::~Float3Item()
	{ }

	void Float3Item::CreateWidgets()
	{
		Widgets::Layout* pLayout = new Widgets::Layout();
		pLayout->SetDirection(Widgets::Layout::Direction::Horizontal);
		pLayout->SetSizeStyle(Widgets::HSIZE_DEFAULT | Widgets::VSIZE_FIT);
		pLayout->GetDefaultStyle().SetBorderSize(1);
		pLayout->GetDefaultStyle().SetBorderColor(Widgets::Color(255, 0, 0, 255));
		pLayout->GetDefaultStyle().ShowBorder(true);

		for (int ii = 0; ii < 3; ++ii)
		{
			m_pTextbox[ii] = new Widgets::TextBox();
			m_pTextbox[ii]->SetSizeStyle(Widgets::DEFAULT);
			m_pTextbox[ii]->SetReadOnly(m_pField->IsReadOnly());
			m_pTextbox[ii]->SetWidth(40);
			m_pTextbox[ii]->OnValidate([ii, this](const std::string& value)
				{
					ObjectWatcher::OPERATION op = m_pField->GetType()->IsContainer() ? ObjectWatcher::SET_ELEMENT : ObjectWatcher::SET_FIELD;

					float newValue = 0;
					std::from_chars(value.c_str(), value.c_str() + value.size(), newValue);

					const Core::Float3* pValue = GetDataPtr<const Core::Float3>();
					Core::Float3 copy = *pValue;
					float* pFloatPtr = reinterpret_cast<float*>(&copy);
					pFloatPtr[ii] = newValue;
					ObjectWatcher::Get().ModifyField(m_pObj, m_pField, op, m_index, &copy);
				});
			pLayout->AddWidget(m_pTextbox[ii]);
		}

		UpdateValue();

		m_pNameWidget = CreateDefaultItemLabel();
		m_pEditingWidget = pLayout;
	}

	void Float3Item::UpdateValue()
	{
		const Core::Float3* pValue = GetDataPtr<const Core::Float3>();
		const float* pFloatPtr = reinterpret_cast<const float*>(pValue);

		for (int ii = 0; ii < 3; ++ii)
		{
			float fValue = pFloatPtr[ii];

			const int BUFFER_SIZE = 16;
			char buffer[BUFFER_SIZE] = { '\0' };
			snprintf(buffer, BUFFER_SIZE, "%g", fValue); //%g removes the meaningless 0.

			m_pTextbox[ii]->SetText(buffer);
		}
	}
}
