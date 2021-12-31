/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Widget.h"

class Layout : public Widget
{
public:
	Layout(uint32_t w, uint32_t h, int32_t x, int32_t y);
	virtual ~Layout();

	void Resize(const DirectX::XMINT3& parentAbsPos, const DirectX::XMUINT2& parentSize) override;

	void Draw() override;
};
