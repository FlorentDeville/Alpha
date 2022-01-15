/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Widgets/Widget.h"

//class Label;
class Layout;

namespace Widgets
{
	class Container;

	class Tab : public Widget
	{
	public:
		Tab();
		~Tab();

		bool Handle(const Message& msg) override;

	};
}
