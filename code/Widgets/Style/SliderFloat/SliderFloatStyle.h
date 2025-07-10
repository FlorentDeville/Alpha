/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Widgets/Style/BaseStyle.h"
#include "Widgets/Tools/Color.h"

namespace Widgets
{
	class SliderFloatStyle : public BaseStyle
	{
		friend class SliderFloat;

	public:
		SliderFloatStyle();
		~SliderFloatStyle();

		void SetShowBorder(bool show);
		void SetBackgroundColor(const Color& color);
		void SetBorderColor(const Color& color);
		void SetBorderSize(int size);

		void SetSliderColor(const Color& color);

	private:
		Color m_backgroundColor;
		Color m_borderColor;
		Color m_sliderColor;
		int m_borderSize;
		bool m_showBorder;
	};
}