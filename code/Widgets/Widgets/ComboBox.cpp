/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Widgets/Widgets/ComboBox.h"

#include "Rendering/PipelineState/PipelineStateMgr.h"

#include "Widgets/Button.h"
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
		, m_displayOptions(false)
	{
		SetSize(Core::UInt2(100, 22));
		SetSizeStyle(Widget::HSIZE_STRETCH | Widget::VSIZE_DEFAULT);
		
		Widgets::Layout* pLayout = new Widgets::Layout(Widgets::Layout::Horizontal_Reverse, Widget::STRETCH);
		AddWidget(pLayout);

		const Widgets::WidgetMgr& widgetMgr = Widgets::WidgetMgr::Get();
		Rendering::TextureId expandedIcon = widgetMgr.GetIconTextureId(Widgets::IconId::kIconDown);
		Widgets::Icon* pIcon = new Widgets::Icon(expandedIcon);
		pIcon->SetFocusPolicy(Widget::FOCUS_POLICY::NO_FOCUS);
		pIcon->SetPositionStyle(Widget::HPOSITION_STYLE::NONE, Widget::VPOSITION_STYLE::MIDDLE);

		pLayout->AddWidget(pIcon);
		pLayout->GetDefaultStyle().SetBackgroundColor(Widgets::Color(0, 0, 0, 0));
		pLayout->GetHoverStyle().SetBackgroundColor(Widgets::Color(0, 0, 0, 0));
		pLayout->SetFocusPolicy(Widget::FOCUS_POLICY::NO_FOCUS);

		m_pLabel = new Label();
		m_pLabel->SetFocusPolicy(Widget::FOCUS_POLICY::NO_FOCUS);
		pLayout->AddWidget(m_pLabel);

		m_pOptionsContainer = new Container();
		m_pOptionsContainer->SetSizeStyle(Widget::HSIZE_DEFAULT | Widget::VSIZE_FIT);
		m_pOptionsContainer->GetDefaultStyle().SetBorderSize(1);
		m_pOptionsContainer->GetDefaultStyle().ShowBorder(true);
		m_pOptionsContainer->GetDefaultStyle().SetBackgroundColor(Widgets::Color(46, 46, 46));
		m_pOptionsContainer->GetDefaultStyle().SetBorderColor(Widgets::Color(66, 66, 66));
		m_pOptionsContainer->SetFocusPolicy(Widget::FOCUS_POLICY::NO_FOCUS);

		m_pOptionsLayout = new Layout(Layout::Vertical, Widget::SIZE_STYLE(Widget::HSIZE_STRETCH | Widget::VSIZE_FIT));
		m_pOptionsLayout->GetDefaultStyle().SetBackgroundColor(Widgets::Color(0, 0, 0, 0));
		m_pOptionsLayout->GetHoverStyle().SetBackgroundColor(Widgets::Color(0, 0, 0, 0));
		m_pOptionsContainer->AddWidget(m_pOptionsLayout);

		GetDefaultStyle().SetBorderSize(1);
		GetDefaultStyle().ShowBorder(true);
		GetDefaultStyle().SetBackgroundColor(Widgets::Color(56, 56, 56));
		GetDefaultStyle().SetBorderColor(Widgets::Color(66, 66, 66));

		GetHoverStyle().SetBorderSize(1);
		GetHoverStyle().ShowBorder(true);
		GetHoverStyle().SetBackgroundColor(Widgets::Color(61, 61, 61));
		GetHoverStyle().SetBorderColor(Widgets::Color(66, 66, 66));

		OnFocusLost([this](const Widgets::FocusEvent& ev) { m_displayOptions = false; });
	}

	ComboBox::~ComboBox()
	{
		delete m_pOptionsContainer;
		m_pOptionsContainer = nullptr;
		m_pOptionsLayout = nullptr;
	}

	void ComboBox::Draw(const Core::Float2& windowSize, const D3D12_RECT& scissor)
	{
		Container::Draw(windowSize, scissor);

		if (m_displayOptions)
		{
			D3D12_RECT rect;
			rect.left = m_pOptionsContainer->GetScreenX();
			rect.right = rect.left + m_pOptionsContainer->GetWidth();
			rect.top = m_pOptionsContainer->GetScreenY();
			rect.bottom = rect.top + m_pOptionsContainer->GetHeight();
			m_pOptionsContainer->Draw(windowSize, rect);
		}
		
	}

	bool ComboBox::Handle(const BaseEvent& ev)
	{
		switch (ev.m_id)
		{
		case Widgets::EventType::kMouseDown:
			m_displayOptions = !m_displayOptions;

			if (m_displayOptions)
			{
				SetFocus();
				Core::Int3 pos = m_absPos;
				pos.y += m_size.y;
				pos.z = 10; // put it in front of everything
				m_pOptionsContainer->SetWidth(m_size.x);
				m_pOptionsContainer->Resize(pos, Core::UInt2(1000, 1000));
				m_pOptionsContainer->SetHeight(m_pOptionsContainer->GetHeight() + 2);
			}
			return true;
			break;
		}

		return Container::Handle(ev);
	}

	void ComboBox::Enable(bool recursive)
	{
		Parent::Enable(recursive);
		m_pOptionsContainer->Enable(true);
		m_displayOptions = false;
	}

	void ComboBox::Disable(bool recursive)
	{
		Parent::Disable(recursive);
		m_pOptionsContainer->Disable(true);
		m_displayOptions = false;
	}

	void ComboBox::AddOption(const std::string& label, int64_t value)
	{
		Option newOption;
		newOption.m_label = label;
		newOption.m_value = value;

		int32_t optionIndex = m_options.GetSize();

		m_options.PushBack(newOption);

		Button* pButton = new Button(label);
		pButton->SetSizeStyle(Widget::HSIZE_STRETCH);
		pButton->OnClick([this, optionIndex]() { InternalSetSelection(optionIndex); });
		m_pOptionsLayout->AddWidget(pButton);
	}

	void ComboBox::SetSelection(int64_t value)
	{
		uint32_t newOption = 0;
		for(uint32_t ii = 0; ii < m_options.GetSize(); ++ii)
		{
			if (m_options[ii].m_value == value)
			{
				newOption = ii;
				break;
			}
		}

		InternalSetSelection(newOption);
	}

	void ComboBox::InternalSetSelection(uint32_t optionIndex)
	{
		if (m_options.IsValidIndex(optionIndex))
		{
			m_selectedOptionIndex = optionIndex;
			m_pLabel->SetText(" " + m_options[optionIndex].m_label); //add a space character cause it's an easy way to put a padding
		}
	}
}
