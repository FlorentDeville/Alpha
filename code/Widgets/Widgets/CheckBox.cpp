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
		, m_value(value)
	{
		Container* pContainer = new Container(11, 11);
		ContainerStyle& defaultStyle = pContainer->GetDefaultStyle();
		defaultStyle.SetBorderSize(1);
		defaultStyle.ShowBorder(true);
		defaultStyle.SetBorderColor(Color(255, 255, 255, 255));
		pContainer->GetHoverStyle() = defaultStyle;
		pContainer->SetPositionStyle(Widget::HPOSITION_STYLE::NONE, Widget::VPOSITION_STYLE::MIDDLE);
		AddWidget(pContainer);

		m_pLabel = new Label(text);
		m_pLabel->SetSizeStyle(Widget::SIZE_STYLE::FIT);
		m_pLabel->SetX(15);
		m_pLabel->SetY(5);
		m_pLabel->SetPositionStyle(Widget::HPOSITION_STYLE::NONE, Widget::VPOSITION_STYLE::NONE);
		AddWidget(m_pLabel);

		ContainerStyle& style = GetDefaultStyle();
		style.SetBorderSize(1);
		style.ShowBorder(true);
		style.SetBorderColor(Color(255, 0, 0, 255));
	}

	CheckBox::~CheckBox()
	{ }

	/*void CheckBox::Draw(const Core::Float2& windowSize, const D3D12_RECT& scissor)
	{ }*/

	void CheckBox::SetValue(bool value)
	{
		m_value = value;
	}
}
