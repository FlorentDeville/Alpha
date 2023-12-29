/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Widgets/Widget.h"

#include "Widgets/Tools/Color.h"

namespace Widgets
{
	class Container;
	class Layout;
	class Tab;

	class TabContainer : public Widget
	{
	public:
		TabContainer();
		~TabContainer();

		void AddWidget(Widget* pWidget) override;

		Container* AddTab(const std::string& header, Tab* pTab);

		void Draw(const DirectX::XMFLOAT2& windowSize) override;

		void SetSelectedTab(int index);

		void Enable(bool recursive = true) override;
		void Disable(bool recursive = true) override;

	private:
		void OnMouseDown_TitleContainer(const MouseEvent& ev, int tabIndex);

		int m_selectedTab;

		Layout* m_pInternalLayout;
		Layout* m_pHeaderLayout;
		Container* m_pContentContainer;

		std::vector<Container*> m_tabHeaders;
		std::vector<Widget*> m_tabContent;

		Color m_defaultHeaderColor;
		Color m_selectedHeaderColor;
	};
}
