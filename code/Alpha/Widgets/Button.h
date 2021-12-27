/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Widget.h"

class Button : public Widget
{
public:
	Button(uint32_t w, uint32_t h, int32_t x, int32_t y);
	virtual ~Button();

	void Draw(int32_t parentX, int32_t parentY, float parentZ) override;

	bool Handle(const Message& msg) override;
};
