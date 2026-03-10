/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Widgets/Widgets/CheckBox.h"

#include "Widgets/Container.h"
#include "Widgets/Label.h"
#include "Widgets/Style/Container/ContainerStyle.h"

namespace Widgets
{
	CheckBox::CheckBox(const std::string& text, bool value)
		: Parent()
		, m_pContainer(nullptr)
		, m_pLabel(nullptr)
		, m_value(value)
		, m_showLabel(true)
	{
		m_pContainer = new Container(11, 11);
		ContainerStyle& defaultStyle = m_pContainer->GetDefaultStyle();
		defaultStyle.SetBorderSize(1);
		defaultStyle.ShowBorder(true);
		defaultStyle.SetBorderColor(Color(255, 255, 255, 255));
		m_pContainer->GetHoverStyle() = defaultStyle;
		m_pContainer->SetPositionStyle(Widget::HPOSITION_STYLE::NONE, Widget::VPOSITION_STYLE::NONE);
		m_pContainer->SetY(4);
		AddWidget(m_pContainer);

		m_pLabel = new Label(text);
		m_pLabel->SetSizeStyle(Widget::SIZE_STYLE::FIT);
		m_pLabel->SetX(15);
		m_pLabel->SetPositionStyle(Widget::HPOSITION_STYLE::NONE, Widget::VPOSITION_STYLE::NONE);
		AddWidget(m_pLabel);

		m_defaultSize.y = 20;
	}

	CheckBox::~CheckBox()
	{ }

	void CheckBox::ReComputeSize(const Core::UInt2& parentSize)
	{
		if (m_showLabel)
		{
			m_size.x = m_pLabel->GetPosition().x + m_pLabel->GetSize().x + 1; //+1 is a margin to make sure the text is inside
			m_size.y = m_defaultSize.y;
		}
		else
		{
			m_size.x = m_pContainer->GetPosition().x + m_pContainer->GetSize().x + 2; //+2 is a margin so the entire container is inside.
			m_size.y = m_defaultSize.y;
		}
	}

	void CheckBox::Enable(bool recursive)
	{
		Parent::Enable(recursive);

		if (!m_showLabel)
			m_pLabel->Disable();
	}

	void CheckBox::SetValue(bool value)
	{
		m_value = value;
	}

	void CheckBox::ShowLabel(bool show)
	{
		if (show == m_showLabel)
			return;

		m_showLabel = show;
		if (show)
			m_pLabel->Enable();
		else
			m_pLabel->Disable();
	}
}
