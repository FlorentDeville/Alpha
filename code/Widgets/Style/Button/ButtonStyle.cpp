/********************************************************************/
/* � 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Widgets/Style/Button/ButtonStyle.h"

namespace Widgets
{
	ButtonStyle::ButtonStyle()
		: BaseStyle()
		, m_borderSize(1)
		, m_backgroundColor(0.12f, 0.12f, 0.12f, 1.f)
		, m_borderColor(1.f, 1.f, 1.f, 1.f)
		, m_showBorder(false)
	{}

	ButtonStyle::~ButtonStyle()
	{}

	void ButtonStyle::SetBorderSize(int size)
	{
		m_borderSize = size;
	}

	void ButtonStyle::SetBackgroundColor(const Color& backgroundColor)
	{
		m_backgroundColor = backgroundColor;
	}

	void ButtonStyle::SetBorderColor(const Color& borderColor)
	{
		m_borderColor = borderColor;
	}

	void ButtonStyle::ShowBorder(bool showBorder)
	{
		m_showBorder = showBorder;
	}
}
