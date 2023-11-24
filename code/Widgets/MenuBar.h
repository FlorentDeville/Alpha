/********************************************************************/
/* © 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Widgets/Widget.h"

namespace Widgets
{
	class Button;
	class Layout;
	class Menu;

	class MenuBar : public Widget
	{
	public:
		MenuBar();
		~MenuBar();

		Menu* AddMenu(const std::string& name);

	private:
		Layout* m_pLayout;
		std::vector<Button*> m_buttons;
	};
}
