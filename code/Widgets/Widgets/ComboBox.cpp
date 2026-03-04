/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Widgets/Widgets/ComboBox.h"

#include "Rendering/PipelineState/PipelineStateMgr.h"

#include "Widgets/Icon.h"
#include "Widgets/Label.h"
#include "Widgets/Layout.h"
#include "Widgets/WidgetMgr.h"

namespace Widgets
{
	ComboBox::ComboBox()
		: Container()
		, m_options()
		, m_selectedOptionIndex(0)
	{
		SetSize(Core::UInt2(100, 22));
		SetSizeStyle(Widget::HSIZE_STRETCH | Widget::VSIZE_DEFAULT);
		
		Widgets::Layout* pLayout = new Widgets::Layout(Widgets::Layout::Horizontal_Reverse, Widget::STRETCH);
		AddWidget(pLayout);

		const Widgets::WidgetMgr& widgetMgr = Widgets::WidgetMgr::Get();
		Rendering::TextureId expandedIcon = widgetMgr.GetIconTextureId(Widgets::IconId::kIconExpanded);
		Widgets::Icon* pIcon = new Widgets::Icon(expandedIcon);
		pLayout->AddWidget(pIcon);
		pLayout->GetDefaultStyle().SetBackgroundColor(Widgets::Color(0, 0, 0, 0));
		pLayout->GetHoverStyle().SetBackgroundColor(Widgets::Color(0, 0, 0, 0));

		m_pLabel = new Label();
		pLayout->AddWidget(m_pLabel);

		GetDefaultStyle().SetBorderSize(1);
		GetDefaultStyle().ShowBorder(true);
		GetDefaultStyle().SetBackgroundColor(Widgets::Color(56, 56, 56));
		GetDefaultStyle().SetBorderColor(Widgets::Color(66, 66, 66));

		GetHoverStyle().SetBorderSize(1);
		GetHoverStyle().ShowBorder(true);
		GetHoverStyle().SetBackgroundColor(Widgets::Color(61, 61, 61));
		GetHoverStyle().SetBorderColor(Widgets::Color(66, 66, 66));
	}

	ComboBox::~ComboBox()
	{ }

	/*void ComboBox::Resize(const Core::Int3& parentAbsPos, const Core::UInt2& parentSize)
	{
		Widget::Resize(parentAbsPos, parentSize);
	}*/

	bool ComboBox::Handle(const BaseEvent& ev)
	{
		return Container::Handle(ev);
	}

	void ComboBox::AddOption(const std::string& label, int64_t value)
	{
		Option newOption;
		newOption.m_label = label;
		newOption.m_value = value;

		m_options.PushBack(newOption);
	}

	void ComboBox::SetSelection(int64_t value)
	{
		for(uint32_t ii = 0; ii < m_options.GetSize(); ++ii)
		{
			if (m_options[ii].m_value == value)
			{
				m_selectedOptionIndex = ii;
				break;
			}
		}

		if (m_options.IsValidIndex(m_selectedOptionIndex))
		{
			m_pLabel->SetText(m_options[m_selectedOptionIndex].m_label);
		}
	}
}
