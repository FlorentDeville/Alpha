/********************************************************************/
/* © 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Widgets/MenuItem.h"

#include "Widgets/Button.h"
#include "Widgets/Label.h"
#include "Widgets/Layout.h"

namespace Widgets
{
	MenuItem::MenuItem(const std::string& name)
		: Button(0, 0, 0, 0)
	{
		SetFocusPolicy(Widget::FOCUS_POLICY::NO_FOCUS);
		SetSizeStyle(Widget::SIZE_STYLE::FIT);
		const int HEIGHT = 20;

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
			m_pShortcut = new Label();
			m_pShortcut->SetSizeStyle(Widget::SIZE_STYLE::HSIZE_DEFAULT | Widget::SIZE_STYLE::VSIZE_FIT);
			m_pShortcut->SetFocusPolicy(Widget::FOCUS_POLICY::NO_FOCUS);
			DirectX::XMUINT2 size = m_pShortcut->GetSize();
			size.x = 50;
			size.y = HEIGHT;
			m_pShortcut->SetSize(size);

			m_pShortcut->SetX(101);
			m_pShortcut->SetY(1);

			AddWidget(m_pShortcut);
		}
	}

	MenuItem::~MenuItem()
	{}

	void MenuItem::AddShortcut(const std::string& shortcut)
	{
		m_pShortcut->SetText(shortcut);
	}
}
