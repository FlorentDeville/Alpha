/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Widget.h"

class HLayout : public Widget
{
public:
	HLayout(uint32_t w, uint32_t h, int32_t x, int32_t y);
	virtual ~HLayout();

	void Resize(const DirectX::XMINT3& parentAbsPos, const DirectX::XMUINT2& parentSize) override;

	void Draw(int32_t parentX, int32_t parentY, float parentZ) override;
};
