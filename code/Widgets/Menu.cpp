/********************************************************************/
/* © 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Widgets/Menu.h"

#include "Widgets/Button.h"
#include "Widgets/Layout.h"
#include "Widgets/MenuItem.h"

namespace Widgets
{
	Menu::Menu()
		: Container()
	{
		SetSizeStyle(Widget::FIT);

		m_pLayout = new Layout();
		m_pLayout->SetSizeStyle(Widget::FIT);
		m_pLayout->GetDefaultStyle().ShowBorder(true);
		m_pLayout->GetDefaultStyle().SetBorderColor(Color(0.44f, 0.44f, 0.44f, 1.f));
		m_pLayout->GetDefaultStyle().SetBackgroundColor(Color(0.24f, 0.24f, 0.24f, 1.f));
		m_pLayout->GetHoverStyle().ShowBorder(true);
		m_pLayout->GetHoverStyle().SetBorderColor(Color(0.44f, 0.44f, 0.44f, 1.f));
		m_pLayout->GetHoverStyle().SetBackgroundColor(Color(0.24f, 0.24f, 0.24f, 1.f));
		m_pLayout->SetDirection(Layout::Direction::Vertical);
		m_pLayout->SetFocusPolicy(Widget::FOCUS_POLICY::NO_FOCUS);
		AddWidget(m_pLayout);
	}

	Menu::~Menu()
	{}

	bool Menu::Handle(const BaseEvent& ev)
	{
		//return true so the widgets behind the menu don't get any event.
		Widget::Handle(ev);
		return true;
	}

	void Menu::ReComputePosition(const Core::Int3& parentAbsPos, const Core::UInt2& parentSize)
	{
		Container::ReComputePosition(parentAbsPos, parentSize);
		m_absPos.z = 10;
	}

	MenuItem* Menu::AddMenuItem(const std::string& itemName)
	{
		MenuItem* pNewItem = new MenuItem(itemName);
		pNewItem->OnClick([this]() { OnClickMenuItem(); });
		m_pLayout->AddWidget(pNewItem);

		return pNewItem;
	}

	void Menu::AddSeparator()
	{
		Container* pContainer = new Container(0, 2);
		pContainer->GetDefaultStyle().SetBackgroundColor(Color(0.18f, 0.18f, 0.18f));
		m_pLayout->AddWidget(pContainer);
	}

	void Menu::OpenMenu()
	{
		Enable();
		SetFocus();
		if (m_onOpen)
			m_onOpen();
	}

	void Menu::CloseMenu()
	{
		Disable();
		if (m_onClose)
			m_onClose();
	}

	Core::CallbackId Menu::OnOpen(const OnOpenEvent::Callback& callback)
	{
		return m_onOpen.Connect(callback);
	}

	Core::CallbackId Menu::OnClose(const OnCloseEvent::Callback& callback)
	{
		return m_onClose.Connect(callback);
	}

	void Menu::OnClickMenuItem()
	{
		CloseMenu();
	}
}
