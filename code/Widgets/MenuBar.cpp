/********************************************************************/
/* © 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Widgets/MenuBar.h"

#include "Widgets/Button.h"
#include "Widgets/Label.h"
#include "Widgets/Layout.h"

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
		Button* pNewButton = new Button(name);
		pNewButton->GetHoverStyle().SetBorderColor(DirectX::XMVectorSet(0.44f, 0.44f, 0.44f, 1.f));

		m_pLayout->AddWidget(pNewButton);

		return nullptr;
	}
}
