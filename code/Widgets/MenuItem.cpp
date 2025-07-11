/********************************************************************/
/* © 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Widgets/MenuItem.h"

#include "Widgets/Button.h"
#include "Widgets/Events/BaseEvent.h"
#include "Widgets/Icon.h"
#include "Widgets/Label.h"
#include "Widgets/Layout.h"
#include "Widgets/Shortcuts/Shortcut.h"
#include "Widgets/WidgetMgr.h"

namespace Widgets
{
	MenuItem::MenuItem(const std::string& name)
		: Button(0, 0, 0, 0)
		, m_pShortcut(nullptr)
		, m_isChecked(false)
	{
		SetFocusPolicy(Widget::FOCUS_POLICY::NO_FOCUS);
		SetSizeStyle(Widget::SIZE_STYLE::FIT);
		const int HEIGHT = 20;
		GetHoverStyle().SetBorderColor(Color(0.48f, 0.48f, 0.48f, 1.f));

		const int CHECKBOX_WIDTH = 12;
		const int LABEL_WIDTH = 150;
		const int SHORTCUT_WIDTH = 50;

		const int CHECK_BOX_X = 5;
		const int LABEL_X = CHECK_BOX_X + CHECKBOX_WIDTH;
		const int SHORTCUT_X = LABEL_X + LABEL_WIDTH;

		//checkbox icon
		{
			m_pCheckmarkIcon = new Icon(WidgetMgr::Get().GetIconTextureId(IconId::kIconCheckmark));
			m_pCheckmarkIcon->SetSizeStyle(SIZE_STYLE::FIT);
			m_pCheckmarkIcon->SetX(CHECK_BOX_X);
			m_pCheckmarkIcon->SetY((HEIGHT - CHECKBOX_WIDTH) / 2);
			m_pCheckmarkIcon->Disable();
			AddWidget(m_pCheckmarkIcon);
		}

		{
			m_pLabel = new Label(name);
			m_pLabel->SetSizeStyle(Widget::SIZE_STYLE::DEFAULT);
			m_pLabel->SetFocusPolicy(Widget::FOCUS_POLICY::NO_FOCUS);
			Core::UInt2 size = m_pLabel->GetSize();
			size.x = LABEL_WIDTH;
			size.y = HEIGHT;
			m_pLabel->SetSize(size);

			m_pLabel->SetX(25);
			m_pLabel->SetY(1);
			AddWidget(m_pLabel);
		}

		{
			m_pShortcutLabel = new Label();
			m_pShortcutLabel->SetSizeStyle(Widget::SIZE_STYLE::HSIZE_DEFAULT | Widget::SIZE_STYLE::VSIZE_FIT);
			m_pShortcutLabel->SetFocusPolicy(Widget::FOCUS_POLICY::NO_FOCUS);
			Core::UInt2 size = m_pShortcutLabel->GetSize();
			size.x = SHORTCUT_WIDTH;
			size.y = HEIGHT;
			m_pShortcutLabel->SetSize(size);

			m_pShortcutLabel->SetX(SHORTCUT_X);
			m_pShortcutLabel->SetY(1);

			AddWidget(m_pShortcutLabel);
		}
	}

	MenuItem::~MenuItem()
	{
		if (m_pShortcut)
		{
			Widgets::WidgetMgr& widgetMgr = Widgets::WidgetMgr::Get();
			widgetMgr.UnregisterShortcut(m_pShortcut);
			delete m_pShortcut;
		}
	}

	bool MenuItem::Handle(const BaseEvent& ev)
	{
		if (ev.m_id == EventType::kMouseUp)
		{
			const MouseEvent& mouseEvent = static_cast<const MouseEvent&>(ev);
			if (mouseEvent.HasButton(MouseButton::LeftButton))
			{
				m_hover = false;
			}
		}

		return ParentClass::Handle(ev);
	}

	void MenuItem::Enable(bool recursive)
	{
		ParentClass::Enable(recursive);

		if (!m_isChecked)
			m_pCheckmarkIcon->Disable();
	}

	void MenuItem::SetShortcut(const std::string& shortcut)
	{
		Widgets::WidgetMgr& widgetMgr = Widgets::WidgetMgr::Get();

		if (m_pShortcut)
		{
			widgetMgr.UnregisterShortcut(m_pShortcut);
			delete m_pShortcut;
		}

		m_pShortcutLabel->SetText(shortcut);

		m_pShortcut = new Shortcut(shortcut, this);
		m_pShortcut->OnActivated([this]() { m_onClick(); });
		widgetMgr.RegisterShortcut(m_pShortcut);
	}

	void MenuItem::SetChecked(bool check)
	{
		m_isChecked = check;
	}

	void MenuItem::SetText(const std::string& text)
	{
		m_pLabel->SetText(text);
	}

	bool MenuItem::IsChecked() const
	{
		return m_isChecked;
	}
}
