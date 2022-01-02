/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Widgets/Widget.h"

namespace Widgets
{
	class Viewport : public Widget
	{
	public:
		Viewport();
		~Viewport();

		void Draw() override;
	};
}