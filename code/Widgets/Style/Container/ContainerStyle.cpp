/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Widgets/Style/Container/ContainerStyle.h"

namespace Widgets
{
	ContainerStyle::ContainerStyle()
		: BaseStyle()
		, m_backgroundColor(0.12f, 0.12f, 0.12f, 1.f)
		, m_borderColor(1.f, 1.f, 1.f, 1.f)
		, m_showBorder(false)
		, m_borderSize(1)
	{}

	ContainerStyle::~ContainerStyle()
	{}

	void ContainerStyle::SetBorderSize(int size)
	{
		m_borderSize = size;
	}

	void ContainerStyle::SetBackgroundColor(const Color& backgroundColor)
	{
		m_backgroundColor = backgroundColor;
	}

	void ContainerStyle::SetBorderColor(const Color& borderColor)
	{
		m_borderColor = borderColor;
	}

	void ContainerStyle::ShowBorder(bool showBorder)
	{
		m_showBorder = showBorder;
	}
}
