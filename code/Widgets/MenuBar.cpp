/********************************************************************/
/* � 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Widgets/MenuBar.h"

#include "Widgets/Button.h"
#include "Widgets/Events/FocusEvent.h"
#include "Widgets/Label.h"
#include "Widgets/Layout.h"
#include "Widgets/Menu.h"
#include "Widgets/WidgetMgr.h"

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
		pNewButton->GetHoverStyle().SetBorderColor(Color(0.44f, 0.44f, 0.44f, 1.f));

		m_pLayout->AddWidget(pNewButton);

		m_buttonsArray.push_back(pNewButton);

		//now create the real menu
		Menu* pNewMenu = new Menu();
		pNewMenu->Disable();
		m_menusArray.push_back(pNewMenu);
		AddWidget(pNewMenu);

		pNewButton->OnClick([this, pNewButton, pNewMenu]() -> bool { return OnClick_MenuButton(pNewButton, pNewMenu); });
		pNewMenu->OnFocusLost([this, pNewButton, pNewMenu](const FocusEvent& ev) { OnLoseFocus_Menu(ev, pNewButton, pNewMenu); });
		pNewMenu->OnOpen([this, pNewButton]() { OnOpen_Menu(pNewButton); });
		pNewMenu->OnClose([this, pNewButton]() { OnClose_Menu(pNewButton); });
		return pNewMenu;
	}

	void MenuBar::Resize(const Core::Int3& parentAbsPos, const Core::UInt2& parentSize)
	{
		Widget::Resize(parentAbsPos, parentSize);

		//recompute the position of every menu
		for(int ii = 0; ii < m_menusArray.size(); ++ii)
		{
			Menu* pMenu = m_menusArray[ii];
			Button* pButton = m_buttonsArray[ii];

			Core::Int3 position;
			position.x = pButton->GetScreenX();
			position.y = pButton->GetScreenY() + pButton->GetHeight();
			position.z = 10;

			pMenu->Resize(position, parentSize);
		}
	}

	void MenuBar::ResizeChildren()
	{
		m_pLayout->Resize(m_absPos, m_size);
	}

	void MenuBar::ReComputeSize_PostChildren()
	{
		if (m_sizeStyle & Widgets::Widget::HSIZE_FIT)
		{
			int max = m_pLayout->GetWidth() + m_pLayout->GetX();
			m_size.x = max;
		}

		if (m_sizeStyle & Widgets::Widget::VSIZE_FIT)
		{
			int max = m_pLayout->GetHeight() + m_pLayout->GetY();
			m_size.y = max;
		}
	}

	void MenuBar::Enable(bool recursive)
	{
		Widget::Enable(false);
		m_pLayout->Enable(recursive);
	}

	void MenuBar::Disable(bool recursive)
	{
		Widget::Disable(false);
		for (Menu* pMenu : m_menusArray)
			pMenu->Disable();

		m_pLayout->Disable(recursive);

	}

	void MenuBar::Draw(const Core::Float2& windowSize, const D3D12_RECT& scissor)
	{
		//Widget::Draw(windowSize);
		m_pLayout->Draw(windowSize, scissor);

		for (Menu* pMenu : m_menusArray)
		{
			if (!pMenu->IsEnabled())
				continue;

			pMenu->Draw(windowSize, scissor);
		}
	}

	bool MenuBar::OnClick_MenuButton(Button* pButton, Menu* pMenu)
	{
		if (!pMenu->IsEnabled())
		{
			Widgets::WidgetMgr::Get().RequestResize();
			pMenu->OpenMenu();
			pButton->Select();
		}
		else
		{
			pMenu->CloseMenu();
		}
		
		return true;
	}

	void MenuBar::OnLoseFocus_Menu(const FocusEvent& ev, Button* pButton, Menu* pMenu)
	{
		if (ev.m_pFocusGained == pButton)
			return;

		pMenu->CloseMenu();
	}

	void MenuBar::OnOpen_Menu(Button* pButton)
	{
		pButton->Select();
	}

	void MenuBar::OnClose_Menu(Button* pButton)
	{
		pButton->Unselect();
	}
}
