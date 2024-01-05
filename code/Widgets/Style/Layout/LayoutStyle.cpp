/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Widgets/Style/Layout/LayoutStyle.h"

namespace Widgets
{
	LayoutStyle::LayoutStyle()
		: BaseStyle()
		, m_borderSize(1)
		, m_backgroundColor(Color(0.12f, 0.12f, 0.12f, 1.f))
		, m_borderColor(Color(1.f, 1.f, 1.f, 1.f))
		, m_showBorder(false)
	{}

	LayoutStyle::~LayoutStyle()
	{}

	void LayoutStyle::SetBorderSize(int size)
	{
		m_borderSize = size;
	}

	void LayoutStyle::SetBackgroundColor(const Color& backgroundColor)
	{
		m_backgroundColor = backgroundColor;
	}

	void LayoutStyle::SetBorderColor(const Color& borderColor)
	{
		m_borderColor = borderColor;
	}

	void LayoutStyle::ShowBorder(bool showBorder)
	{
		m_showBorder = showBorder;
	}
}
