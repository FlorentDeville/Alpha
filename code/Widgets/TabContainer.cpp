/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Widgets/TabContainer.h"

#include "Widgets/Container.h"
#include "Widgets/Label.h"
#include "Widgets/Layout.h"
#include "Widgets/Tab.h"
#include "Widgets/WidgetMgr.h"

#include "Rendering/Font/FontMgr.h"

namespace Widgets
{
	TabContainer::TabContainer()
		: Widget()
		, m_selectedTab(0)
		, m_pInternalLayout(nullptr)
		, m_pHeaderLayout(nullptr)
		, m_pContentContainer(nullptr)
	{
		SetSizeStyle(HSIZE_STRETCH | VSIZE_STRETCH);

		m_pInternalLayout = new Layout(0, 0, 0, 0);
		m_pInternalLayout->SetSizeStyle(HSIZE_STRETCH | VSIZE_STRETCH);
		m_pInternalLayout->SetDirection(Layout::Direction::Vertical);
		Widget::AddWidget(m_pInternalLayout);

		m_pHeaderLayout = new Layout(0, 20, 0, 0);
		m_pHeaderLayout->SetSizeStyle(HSIZE_STRETCH | VSIZE_DEFAULT);
		m_pHeaderLayout->SetDirection(Layout::Direction::Horizontal);
		m_pHeaderLayout->SetSpace(DirectX::XMINT2(2, 0));

		m_pContentContainer = new Widgets::Container(100, 100);
		m_pContentContainer->SetSizeStyle(HSIZE_STRETCH | VSIZE_STRETCH);

		m_pInternalLayout->AddWidget(m_pHeaderLayout);
		m_pInternalLayout->AddWidget(m_pContentContainer);

		m_selectedHeaderColor = Color(0.24f, 0.24f, 0.24f, 1.f);
		m_defaultHeaderColor = Color(0.18f, 0.18f, 0.18f, 1.f);
	}

	TabContainer::~TabContainer()
	{}

	void TabContainer::AddWidget(Widget* /*pWidget*/)
	{
		assert(false);
	}

	Container* TabContainer::AddTab(const std::string& header, Tab* pTab)
	{
		const Rendering::Font* pFont = Rendering::FontMgr::Get().GetFont(WidgetMgr::Get().GetUIFontId());

		const int DEFAULT_WIDTH = 50;
		const int DEFAULT_HEIGHT = 17;
		DirectX::XMUINT2 textRect(DEFAULT_WIDTH, DEFAULT_HEIGHT);

		int tabIndex = static_cast<int>(m_tabHeaders.size());

		Widgets::Container* pTitleContainer = new Widgets::Container(textRect.x, textRect.y);
		Label* pTitleLabel = new Label(0, 0, 1, header);
		pTitleContainer->AddWidget(pTitleLabel);
		pTitleContainer->OnMouseDown([this, tabIndex](const MouseEvent& ev) { OnMouseDown_TitleContainer(ev, tabIndex); });

		m_pHeaderLayout->AddWidget(pTitleContainer);
		m_pContentContainer->AddWidget(pTab);

		m_selectedTab = static_cast<int>(m_tabContent.size());

		m_tabHeaders.push_back(pTitleContainer);
		m_tabContent.push_back(pTab);

		return pTitleContainer;
	}

	void TabContainer::Draw(const DirectX::XMFLOAT2& windowSize, const D3D12_RECT& scissor)
	{
		Widget::Draw(windowSize, scissor);
	}

	void TabContainer::SetSelectedTab(int index)
	{
		if (m_selectedTab == index)
			return;

		//reset all the other tab to the default state
		for (int ii = 0; ii < m_tabContent.size(); ++ii)
		{
			if (ii == index)
				continue;

			m_tabHeaders[ii]->GetDefaultStyle().SetBackgroundColor(m_defaultHeaderColor);
			m_tabContent[ii]->Disable();
		}

		//Change the header color
		m_tabHeaders[index]->GetDefaultStyle().SetBackgroundColor(m_selectedHeaderColor);

		//Show the new content
		m_tabContent[index]->Enable();

		//Update the selected index
		m_selectedTab = index;
	}

	void TabContainer::Enable(bool recursive)
	{
		Widget::Enable(recursive);

		//Widget::Enable enabled all the tab so I need to hide all unselected tab.
		for (int ii = 0; ii < m_tabContent.size(); ++ii)
		{
			if (ii == m_selectedTab)
				continue;

			m_tabHeaders[ii]->GetDefaultStyle().SetBackgroundColor(m_defaultHeaderColor);
			m_tabContent[ii]->Disable(recursive);
		}
		
	}

	void TabContainer::Disable(bool recursive)
	{
		Widget::Disable(recursive);
	}

	void TabContainer::OnMouseDown_TitleContainer(const MouseEvent& ev, int tabIndex)
	{
		if (!ev.HasButton(MouseButton::LeftButton))
			return;

		SetSelectedTab(tabIndex);
		Widgets::WidgetMgr::Get().RequestResize();
	}
}
