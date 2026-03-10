/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Widgets/Widgets/CheckBox.h"

#include "Widgets/Container.h"
#include "Widgets/Icon.h"
#include "Widgets/Label.h"
#include "Widgets/Style/Container/ContainerStyle.h"
#include "Widgets/WidgetMgr.h"

namespace Widgets
{
	CheckBox::CheckBox(const std::string& text, bool value)
		: Parent()
		, m_pContainer(nullptr)
		, m_pLabel(nullptr)
		, m_value(value)
		, m_showLabel(true)
		, pCheckedIcon(nullptr)
		, m_isReadOnly(false)
	{
		GetDefaultStyle().SetBackgroundColor(Color(0, 0, 0, 0));
		GetHoverStyle().SetBackgroundColor(Color(0, 0, 0, 0));

		const int BOX_SIZE = 12;
		m_pContainer = new Container(BOX_SIZE, BOX_SIZE);
		m_pContainer->SetPositionStyle(Widget::HPOSITION_STYLE::NONE, Widget::VPOSITION_STYLE::NONE);
		m_pContainer->SetY(4);
		AddWidget(m_pContainer);

		m_pLabel = new Label(text);
		m_pLabel->SetSizeStyle(Widget::SIZE_STYLE::FIT);
		m_pLabel->SetX(15);
		m_pLabel->SetPositionStyle(Widget::HPOSITION_STYLE::NONE, Widget::VPOSITION_STYLE::NONE);
		AddWidget(m_pLabel);

		ContainerStyle& hoverStyle = m_pContainer->GetHoverStyle();
		hoverStyle.SetBorderSize(1);
		hoverStyle.ShowBorder(true);
		hoverStyle.SetBorderColor(Color(255, 255, 255, 255));
		hoverStyle.SetBackgroundColor(Color(61, 61, 61, 255));
		m_savedHoverStyle = hoverStyle;

		ContainerStyle& defaultStyle = m_pContainer->GetDefaultStyle();
		defaultStyle.SetBorderSize(1);
		defaultStyle.ShowBorder(true);
		defaultStyle.SetBorderColor(Color(255, 255, 255, 255));
		m_savedDefaultStyle = defaultStyle;

		m_readOnlyStyle.SetBorderColor(Color(91, 91, 91));
		m_readOnlyStyle.ShowBorder(true);
		m_readOnlyStyle.SetBorderSize(1);
		m_readOnlyStyle.SetBackgroundColor(Color(31, 31, 31));

		//checkbox icon
		{
			const int CHECKMARK_SIZE = 10;
			pCheckedIcon = new Icon(WidgetMgr::Get().GetIconTextureId(IconId::kIconCheckmarkWhite));
			pCheckedIcon->SetSizeStyle(SIZE_STYLE::DEFAULT);
			pCheckedIcon->SetWidth(CHECKMARK_SIZE);
			pCheckedIcon->SetHeight(CHECKMARK_SIZE);
			m_pContainer->AddWidget(pCheckedIcon);
		}

		if (value)
		{
			pCheckedIcon->Enable();
		}
		else
		{
			pCheckedIcon->Disable();
		}

		m_defaultSize.y = 20;

		OnMouseDown([this](const MouseEvent& ev) { Internal_OnMouseDown(ev); });
	}

	CheckBox::CheckBox(bool value)
		: CheckBox("", value)
	{
		m_showLabel = false;
		m_pLabel->Disable();
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

		if (!m_value)
			pCheckedIcon->Disable();
	}

	void CheckBox::SetValue(bool value)
	{
		if (m_value == value || m_isReadOnly)
			return;

		m_value = value;

		if (m_value)
		{
			pCheckedIcon->Enable();
		}
		else
		{
			pCheckedIcon->Disable();
		}

		m_onValueChanged(value);
	}

	void CheckBox::SetReadOnly(bool readOnly)
	{
		m_isReadOnly = readOnly;

		if (readOnly)
		{
			m_pContainer->GetHoverStyle() = m_readOnlyStyle;
			m_pContainer->GetDefaultStyle() = m_readOnlyStyle;
		}
		else
		{
			m_pContainer->GetDefaultStyle() = m_savedDefaultStyle;
			m_pContainer->GetHoverStyle() = m_savedHoverStyle;
		}
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

	void CheckBox::Internal_OnMouseDown(const MouseEvent& ev)
	{
		SetValue(!m_value);
	}
}
