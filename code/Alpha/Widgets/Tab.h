/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Widgets/Widget.h"

//class Label;
class Layout;
class Container;

namespace Widgets
{
	class Tab : public Widget
	{
	public:
		Tab(const std::string& title);
		~Tab();

		void AddWidget(Widget* pWidget) override;

	private:
		const std::string m_title;
		Layout* m_pInternalLayout;
	};
}
