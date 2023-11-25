/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Widgets/Style/BaseStyle.h"

#include "Widgets/Tools/Color.h"

namespace Widgets
{
	class ButtonStyle : public BaseStyle
	{
		friend class Button;

	public:
		ButtonStyle();
		~ButtonStyle();

		void SetBorderSize(int size);
		void SetBackgroundColor(const Color& backgroundColor);
		void SetBorderColor(const Color& borderColor);
		void ShowBorder(bool showBorder);

	private:
		Color m_backgroundColor;
		Color m_borderColor;
		int m_borderSize;
		bool m_showBorder;
	};
}
