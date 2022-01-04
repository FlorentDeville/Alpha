/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Widgets/Tab.h"

#include "Widgets/Container.h"
#include "Widgets/Label.h"
#include "Widgets/Layout.h"
#include "Widgets/WidgetMgr.h"

namespace Widgets
{
	Tab::Tab(const std::string& title)
		: Widget()
		, m_title(title)
		, m_pInternalLayout(nullptr)
	{
		SetSizeStyle(HSIZE_STRETCH | VSIZE_STRETCH);

		m_pInternalLayout = new Layout(0, 0, 0, 0);
		m_pInternalLayout->SetSizeStyle(HSIZE_STRETCH | VSIZE_STRETCH);
		m_pInternalLayout->SetDirection(Layout::Direction::Vertical);
		Widget::AddWidget(m_pInternalLayout);

		const Font* pFont = g_pFontMgr->GetResource(g_pWidgetMgr->GetUIFontId());
		DirectX::XMUINT2 textRect;
		pFont->ComputeRect(m_title, textRect);
		
		Container* pTitleContainer = new Container(textRect.x, textRect.y);
		pTitleContainer->SetBackgroundColor(DirectX::XMVectorSet(0.24f, 0.24f, 0.24f, 1.f));
		Label* pTitleLabel = new Label(0, 0, 1, m_title);
		pTitleContainer->AddWidget(pTitleLabel);

		m_pInternalLayout->AddWidget(pTitleContainer);
	}

	Tab::~Tab()
	{}

	void Tab::AddWidget(Widget* pWidget)
	{
		m_pInternalLayout->AddWidget(pWidget);
	}
}
