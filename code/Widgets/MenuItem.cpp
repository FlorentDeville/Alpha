/********************************************************************/
/* © 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Widgets/MenuItem.h"

#include "Widgets/Label.h"
#include "Widgets/Layout.h"

namespace Widgets
{
	MenuItem::MenuItem(const std::string& name)
		: Container()
	{
		SetSizeStyle(Widget::SIZE_STYLE::FIT);

		m_pLayout = new Layout();
		m_pLayout->SetDirection(Layout::Direction::Horizontal);
		m_pLayout->SetSizeStyle(Widget::SIZE_STYLE::FIT);
		AddWidget(m_pLayout);
		
		{
			m_pLabel = new Label(name);
			m_pLabel->SetSizeStyle(Widget::SIZE_STYLE::HSIZE_DEFAULT | Widget::SIZE_STYLE::VSIZE_FIT);
			DirectX::XMUINT2 size = m_pLabel->GetSize();
			size.x = 100;
			m_pLabel->SetSize(size);

			m_pLayout->AddWidget(m_pLabel);
		}

		{
			m_pShortcut = new Label();
			m_pShortcut->SetSizeStyle(Widget::SIZE_STYLE::HSIZE_DEFAULT | Widget::SIZE_STYLE::VSIZE_FIT);
			DirectX::XMUINT2 size = m_pShortcut->GetSize();
			size.x = 30;
			m_pShortcut->SetSize(size);

			m_pLayout->AddWidget(m_pShortcut);
		}
	}

	MenuItem::~MenuItem()
	{}

	void MenuItem::AddShortcut(const std::string& shortcut)
	{
		m_pShortcut->SetText(shortcut);
	}
}
