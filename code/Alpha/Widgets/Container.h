/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Widgets/Widget.h"

namespace Widgets
{

	class Container : public Widget
	{
	public:
		Container();
		~Container();

		void Draw() override;
	};

}