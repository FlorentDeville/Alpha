/********************************************************************/
/* © 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Widgets/MenuBar.h"

#include "Widgets/Button.h"
#include "Widgets/Label.h"
#include "Widgets/Layout.h"
#include "Widgets/Menu.h"

//#pragma optimize("", off)

namespace Widgets
{
	MenuBar::MenuBar()
		:Widget()
	{
		SetSizeStyle(HSIZE_STRETCH | VSIZE_FIT);

		m_pLayout = new Layout();
		m_pLayout->SetDirection(Layout::Direction::Horizontal);
		m_pLayout->SetSizeStyle(HSIZE_STRETCH | VSIZE_FIT);
		AddWidget(m_pLayout);
	}

	MenuBar::~MenuBar()
	{}

	Menu* MenuBar::AddMenu(const std::string& name)
	{
		//create the menu button
		Button* pNewButton = new Button(name);
		pNewButton->GetHoverStyle().SetBorderColor(DirectX::XMVectorSet(0.44f, 0.44f, 0.44f, 1.f));

		m_pLayout->AddWidget(pNewButton);

		m_buttonsArray.push_back(pNewButton);

		//now create the real menu
		Menu* pNewMenu = new Menu();
		pNewMenu->Disable(false);
		pNewMenu->OnLoseFocus([pNewMenu]() -> bool {
			pNewMenu->Disable(false); 
			return true; 
			});
		m_menusArray.push_back(pNewMenu);

		pNewButton->OnClick([pNewMenu](int, int) -> bool {
			pNewMenu->Enable();
			pNewMenu->SetFocus();
			return true;
			});

		return pNewMenu;
	}

	void MenuBar::Resize(const DirectX::XMINT3& parentAbsPos, const DirectX::XMUINT2& parentSize)
	{
		Widget::Resize(parentAbsPos, parentSize);

		//recompute the position of every menu
		for(int ii = 0; ii < m_menusArray.size(); ++ii)
		{
			Menu* pMenu = m_menusArray[ii];
			Button* pButton = m_buttonsArray[ii];

			DirectX::XMINT3 position;
			position.x = pButton->GetScreenX();
			position.y = pButton->GetScreenY() + pButton->GetHeight();
			position.z = 10;
			//pMenu->SetAbsPosition(position.x, position.y);

			pMenu->Resize(position, parentSize);
		}
	}

	void MenuBar::Draw(const DirectX::XMFLOAT2& windowSize)
	{
		Widget::Draw(windowSize);

		for (Menu* pMenu : m_menusArray)
		{
			if (!pMenu->IsEnabled())
				continue;

			pMenu->Draw(windowSize);
		}
	}
}
