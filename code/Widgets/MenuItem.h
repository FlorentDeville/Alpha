/********************************************************************/
/* © 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Widgets/Container.h"

namespace Widgets
{
	class Label;
	class Layout;

	class MenuItem : public Container
	{
	public:
		MenuItem(const std::string& name);
		~MenuItem();

		void AddShortcut(const std::string& shortcut);

	private:
		Layout* m_pLayout;
		Label* m_pLabel;
		Label* m_pShortcut;
	};
}
