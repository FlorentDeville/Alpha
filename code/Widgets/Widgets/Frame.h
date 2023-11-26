/********************************************************************/
/* © 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Widgets/Container.h"

#include <string>

namespace Widgets
{
	class Label;
	class Layout;
	class Widget;

	class Frame : public Container
	{
	public:
		Frame(const std::string& title);
		~Frame();

		void AddWidget(Widget* pWidget) override;

	private:		
		Layout* m_pInternalLayout;
		Label* m_pTitleLabel;
		Layout* m_pTitleLayout;
		Container* m_pInternalContainer;
	};
}
