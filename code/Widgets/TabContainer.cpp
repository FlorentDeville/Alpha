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
	TabContainer::TabContainer(bool headerTop)
		: Widget()
		, m_selectedTab(0)
		, m_pInternalLayout(nullptr)
		, m_pHeaderLayout(nullptr)
		, m_pContentContainer(nullptr)
	{
		SetSizeStyle(HSIZE_STRETCH | VSIZE_STRETCH);

		m_pInternalLayout = new Layout(0, 0, 0, 0);
		m_pInternalLayout->SetSizeStyle(HSIZE_STRETCH | VSIZE_STRETCH);

		if(headerTop)
			m_pInternalLayout->SetDirection(Layout::Direction::Vertical);
		else
			m_pInternalLayout->SetDirection(Layout::Direction::Vertical_Reverse);

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

	Container* TabContainer::AddTab(const std::string& header, Widget* pWidget) 
	{
		const Rendering::Font* pFont = Rendering::FontMgr::Get().GetFont(WidgetMgr::Get().GetUIFontId());

		const int DEFAULT_WIDTH = 50;
		const int DEFAULT_HEIGHT = 17;
		DirectX::XMUINT2 textRect(DEFAULT_WIDTH, DEFAULT_HEIGHT);

		uint64_t tabIndex = m_tabHeaders.size();
		m_tabIdToIndex[SID(header.c_str())] = tabIndex;

		Widgets::Container* pTitleContainer = new Widgets::Container(textRect.x, textRect.y);

		//cheat padding
		Label* pTitleLabel = new Label(" " + header + " ");
		pTitleLabel->SetSizeStyle(Widgets::Widget::HSIZE_FIT | Widgets::Widget::VSIZE_DEFAULT);
		pTitleLabel->SetPositionStyle(Widgets::Widget::HPOSITION_STYLE::CENTER, Widgets::Widget::VPOSITION_STYLE::MIDDLE);
		pTitleContainer->SetSizeStyle(Widgets::Widget::HSIZE_FIT | Widgets::Widget::VSIZE_DEFAULT);
		pTitleContainer->AddWidget(pTitleLabel);
		pTitleContainer->OnMouseDown([this, pTitleContainer](const MouseEvent& ev) { OnMouseDown_TitleContainer(ev, pTitleContainer); });

		m_pHeaderLayout->AddWidget(pTitleContainer);
		m_pContentContainer->AddWidget(pWidget);
		
		m_tabHeaders.push_back(pTitleContainer);
		m_tabContent.push_back(pWidget);

		SetSelectedTab(static_cast<int>(m_tabContent.size() - 1));

		return pTitleContainer;
	}

	void TabContainer::CloseTab(Widget* pWidget)
	{
		std::vector<Widget*>::iterator it = std::find(m_tabContent.begin(), m_tabContent.end(), pWidget);
		if (it == m_tabContent.end())
			return;

		size_t index = std::distance(m_tabContent.begin(), it);

		WidgetMgr::Get().RequestWidgetDeletion(pWidget);
		WidgetMgr::Get().RequestWidgetDeletion(m_tabHeaders[index]);

		m_tabContent.erase(m_tabContent.cbegin() + index);
		m_tabHeaders.erase(m_tabHeaders.cbegin() + index);

		int tabCount = static_cast<int>(m_tabContent.size());

		if (m_selectedTab == index)
		{
			int newIndex = m_selectedTab;
			if (index >= tabCount)
				newIndex = tabCount - 1;

			Internal_SetSelectedTab(newIndex);
		}
		
		for (std::pair<Core::Sid, uint64_t> it : m_tabIdToIndex)
		{
			if (it.second > index)
				it.second = it.second - 1;
		}

		Widgets::WidgetMgr::Get().RequestResize();
	}

	void TabContainer::Draw(const Core::Float2& windowSize, const D3D12_RECT& scissor)
	{
		Widget::Draw(windowSize, scissor);
	}

	void TabContainer::SetSelectedTab(int index)
	{
		if (m_selectedTab == index)
			return;

		Internal_SetSelectedTab(index);
	}

	void TabContainer::SetSelectedTab(Core::Sid tabId)
	{
		std::map<Core::Sid, uint64_t>::const_iterator it = m_tabIdToIndex.find(tabId);
		if (it == m_tabIdToIndex.cend())
			return;

		SetSelectedTab(static_cast<int>(it->second));
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

	void TabContainer::OnMouseDown_TitleContainer(const MouseEvent& ev, Container* pHeader)
	{
		if (!ev.HasButton(MouseButton::LeftButton))
			return;

		std::vector<Container*>::const_iterator it = std::find(m_tabHeaders.cbegin(), m_tabHeaders.cend(), pHeader);
		if (it == m_tabHeaders.cend())
			return;

		size_t index = std::distance(m_tabHeaders.cbegin(), it);
		SetSelectedTab(static_cast<int>(index));
		Widgets::WidgetMgr::Get().RequestResize();
	}

	void TabContainer::Internal_SetSelectedTab(int index)
	{
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

		Widgets::WidgetMgr::Get().RequestResize();
	}
}
