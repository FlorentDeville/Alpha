/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Widgets/Tools/Color.h"
#include "Widgets/Style/BaseStyle.h"

namespace Widgets
{
	class TextBoxStyle : public BaseStyle
	{
		friend class TextBox;

	public:
		TextBoxStyle();
		~TextBoxStyle();

		void SetShowBorder(bool show);
		void SetBackgroundColor(const Color& color);
		void SetBorderColor(const Color& color);
		void SetBorderSize(int size);

	private:
		Color m_backgroundColor;
		Color m_borderColor;
		int m_borderSize;
		bool m_showBorder;
	};
}
