/********************************************************************/
/* © 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Widgets/Menu.h"

#include "Widgets/Button.h"
#include "Widgets/Layout.h"
#include "Widgets/MenuItem.h"
#include "Widgets/Message.h"

namespace Widgets
{
	Menu::Menu()
		: Container()
	{
		SetSizeStyle(Widget::FIT);

		m_pLayout = new Layout();
		m_pLayout->SetSizeStyle(Widget::FIT);
		m_pLayout->GetDefaultStyle().ShowBorder(true);
		m_pLayout->GetDefaultStyle().SetBorderColor(DirectX::XMVectorSet(0.44f, 0.44f, 0.44f, 1.f));
		m_pLayout->GetDefaultStyle().SetBackgroundColor(DirectX::XMVectorSet(0.24f, 0.24f, 0.24f, 1.f));
		m_pLayout->GetHoverStyle().ShowBorder(true);
		m_pLayout->GetHoverStyle().SetBorderColor(DirectX::XMVectorSet(0.44f, 0.44f, 0.44f, 1.f));
		m_pLayout->GetHoverStyle().SetBackgroundColor(DirectX::XMVectorSet(0.24f, 0.24f, 0.24f, 1.f));
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

	void Menu::ReComputePosition(const DirectX::XMINT3& parentAbsPos, const DirectX::XMUINT2& parentSize)
	{
		Container::ReComputePosition(parentAbsPos, parentSize);
		m_absPos.z = 10;
	}

	MenuItem* Menu::AddMenuItem(const std::string& itemName)
	{
		MenuItem* pNewItem = new MenuItem(itemName);
		m_pLayout->AddWidget(pNewItem);

		return pNewItem;
	}
}
