/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Widget.h"

class Layout : public Widget
{
public:
	enum Direction
	{
		Horizontal,				//left to right
		Horizontal_Reverse,		//right to left
		Vertical,				//top to bottom
		//Vertical_Reverse		//bottom to top
	};

	Layout(uint32_t w, uint32_t h, int32_t x, int32_t y);
	virtual ~Layout();

	void Resize(const DirectX::XMINT3& parentAbsPos, const DirectX::XMUINT2& parentSize) override;

	void Draw() override;

	bool Handle(const Message& msg) override;

	void SetDirection(Direction dir);
	void SetSpace(const DirectX::XMINT2& space);

private:
	Direction m_dir;

	DirectX::XMUINT2 m_previousMousePosition;

	DirectX::XMINT2 m_space; //space to add between widgets
};
