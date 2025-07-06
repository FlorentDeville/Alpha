/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Widgets/Layout.h"

#include <string>

namespace Widgets
{
	class Label;
}

namespace Editors
{
	class StatusBar : public Widgets::Layout
	{
	public:
		StatusBar();
		~StatusBar();

		void SetText(const std::string& text);

	private:
		Widgets::Label* m_pLabel;
	};
}