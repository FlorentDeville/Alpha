/********************************************************************/
/* © 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Widgets/MenuItem.h"

#include "Widgets/Button.h"
#include "Widgets/Events/BaseEvent.h"
#include "Widgets/Label.h"
#include "Widgets/Layout.h"
#include "Widgets/Shortcuts/Shortcut.h"
#include "Widgets/WidgetMgr.h"

namespace Widgets
{
	MenuItem::MenuItem(const std::string& name)
		: Button(0, 0, 0, 0)
		, m_pShortcut(nullptr)
	{
		SetFocusPolicy(Widget::FOCUS_POLICY::NO_FOCUS);
		SetSizeStyle(Widget::SIZE_STYLE::FIT);
		const int HEIGHT = 20;
		GetHoverStyle().SetBorderColor(Color(0.48f, 0.48f, 0.48f, 1.f));

		{
			m_pLabel = new Label(name);
			m_pLabel->SetSizeStyle(Widget::SIZE_STYLE::DEFAULT);
			m_pLabel->SetFocusPolicy(Widget::FOCUS_POLICY::NO_FOCUS);
			DirectX::XMUINT2 size = m_pLabel->GetSize();
			size.x = 100;
			size.y = HEIGHT;
			m_pLabel->SetSize(size);

			m_pLabel->SetX(5);
			m_pLabel->SetY(1);
			AddWidget(m_pLabel);
		}

		{
			m_pShortcutLabel = new Label();
			m_pShortcutLabel->SetSizeStyle(Widget::SIZE_STYLE::HSIZE_DEFAULT | Widget::SIZE_STYLE::VSIZE_FIT);
			m_pShortcutLabel->SetFocusPolicy(Widget::FOCUS_POLICY::NO_FOCUS);
			DirectX::XMUINT2 size = m_pShortcutLabel->GetSize();
			size.x = 50;
			size.y = HEIGHT;
			m_pShortcutLabel->SetSize(size);

			m_pShortcutLabel->SetX(101);
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
		switch (ev.m_id)
		{
		case EventId::kMouseLUp:
		{
			if (m_onClick)
				m_onClick();

			m_hover = false;
			return true;
			//shut down parent menu
		}
		break;

		default:
			return Button::Handle(ev);
			break;
		}

		return false;
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

		m_pShortcut = new Shortcut(shortcut);
		m_pShortcut->OnActivated([this]() { m_onClick(); });
		widgetMgr.RegisterShortcut(m_pShortcut);
	}
}
