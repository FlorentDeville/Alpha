/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Widget.h"

#include <functional>

namespace Widgets
{
	class Button : public Widget
	{
	public:
		Button(uint32_t w, uint32_t h, int32_t x, int32_t y);
		virtual ~Button();

		void Draw(const DirectX::XMFLOAT2& windowSize) override;

		bool Handle(const Message& msg) override;

		void Select();
		void Unselect();

	private:
		//if the button is selected, it keeps the hover color until Unselect is called.
		bool m_isSelected;
	};
}
