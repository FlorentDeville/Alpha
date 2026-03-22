/********************************************************************************/
/* Copyright (C) 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Widgets/Widget.h"

namespace Widgets
{
	class Container;

	class Tab : public Widget
	{
	public:
		Tab();
		~Tab();

		bool Handle(const EventStorage& ev) override;
	};
}
