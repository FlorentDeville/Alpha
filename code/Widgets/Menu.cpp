/********************************************************************/
/* © 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Widgets/Menu.h"

#include "Widgets/Button.h"
#include "Widgets/Layout.h"

namespace Widgets
{
	Menu::Menu()
		: Container()
	{
		SetSizeStyle(Widget::FIT);

		m_pLayout = new Layout();
		m_pLayout->SetSizeStyle(Widget::FIT);
		m_pLayout->GetDefaultStyle().ShowBorder(true);
		m_pLayout->SetDirection(Layout::Direction::Vertical);
		AddWidget(m_pLayout);
	}

	Menu::~Menu()
	{}

	void Menu::ReComputePosition(const DirectX::XMINT3& parentAbsPos, const DirectX::XMUINT2& parentSize)
	{
		Container::ReComputePosition(parentAbsPos, parentSize);
		m_absPos.z = 10;
	}

	void Menu::AddMenuItem(const std::string& itemName)
	{
		Button* pNewButton = new Button(itemName);
		m_pLayout->AddWidget(pNewButton);
	}
}
