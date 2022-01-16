/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Widgets/Widget.h"

class Layout;

namespace Widgets
{
	class Container;
	class Tab;

	class TabContainer : public Widget
	{
	public:
		TabContainer();
		~TabContainer();

		void AddWidget(Widget* pWidget) override;

		void AddTab(const std::string& header, Tab* pTab);

		void Draw(const DirectX::XMFLOAT2& windowSize) override;

		void SetSelectedTab(int index);

	private:
		int m_selectedTab;

		Layout* m_pInternalLayout;
		Layout* m_pHeaderLayout;
		Container* m_pContentContainer;

		std::vector<Widget*> m_tabHeaders;
		std::vector<Widget*> m_tabContent;

		DirectX::XMVECTOR m_defaultHeaderColor;
		DirectX::XMVECTOR m_selectedHeaderColor;
	};
}
