/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Widgets/Style/SliderFloat/SliderFloatStyle.h"

namespace Widgets
{
	SliderFloatStyle::SliderFloatStyle()
		: BaseStyle()
		, m_borderSize(1)
		, m_backgroundColor(Color(0.12f, 0.12f, 0.12f, 1.f))
		, m_borderColor(Color(1.f, 1.f, 1.f, 1.f))
		, m_showBorder(false)
	{ }

	SliderFloatStyle::~SliderFloatStyle()
	{ }

	void SliderFloatStyle::SetShowBorder(bool show)
	{
		m_showBorder = show;
	}

	void SliderFloatStyle::SetBackgroundColor(const Color& color)
	{
		m_backgroundColor = color;
	}

	void SliderFloatStyle::SetBorderColor(const Color& color)
	{
		m_borderColor = color;
	}

	void SliderFloatStyle::SetBorderSize(int size)
	{
		m_borderSize = size;
	}

	void SliderFloatStyle::SetSliderColor(const Color& color)
	{
		m_sliderColor = color;
	}
}
