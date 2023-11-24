/********************************************************************/
/* © 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Widgets/Button.h"

#include <string>

namespace Widgets
{
	class Label;

	class MenuItem : public Button
	{
	public:
		MenuItem(const std::string& name);
		~MenuItem();

		void AddShortcut(const std::string& shortcut);

	private:
		Label* m_pLabel;
		Label* m_pShortcut;
	};
}
