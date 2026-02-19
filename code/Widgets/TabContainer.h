/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Widgets/Widget.h"

#include "Core/Sid/Sid.h"
#include "Widgets/Tools/Color.h"

namespace Widgets
{
	class Container;
	class Layout;
	class Tab;

	class TabContainer : public Widget
	{
	public:
		TabContainer(bool headerTop = true);
		~TabContainer();

		void AddWidget(Widget* pWidget) override;

		//The new tab id is SID(header).
		Container* AddTab(const std::string& header, Widget* pWidget);

		void CloseTab(Widget* pWidget);

		void Draw(const Core::Float2& windowSize, const D3D12_RECT& scissor) override;

		void SetSelectedTab(int index);
		void SetSelectedTab(Core::Sid tabId);

		void Enable(bool recursive = true) override;
		void Disable(bool recursive = true) override;

	private:
		void OnMouseDown_TitleContainer(const MouseEvent& ev, Container* pHeader);

		void Internal_SetSelectedTab(int index);

		int m_selectedTab;

		Layout* m_pInternalLayout;
		Layout* m_pHeaderLayout;
		Container* m_pContentContainer;

		std::vector<Container*> m_tabHeaders;
		std::vector<Widget*> m_tabContent;

		std::map<Core::Sid, uint64_t> m_tabIdToIndex; //map between an unique id to identify the tab and its index in the internal arrays.

		Color m_defaultHeaderColor;
		Color m_selectedHeaderColor;
	};
}
