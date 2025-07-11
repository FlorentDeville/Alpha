/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Widgets/Style/TextBox/TextBoxStyle.h"

namespace Widgets
{
	TextBoxStyle::TextBoxStyle()
		: BaseStyle()
		, m_borderSize(1)
		, m_backgroundColor(Color(0.22f, 0.22f, 0.22f, 1.f))
		, m_borderColor(Color(0.26f, 0.26f, 0.26f, 1.f))
		, m_showBorder(true)
	{}

	TextBoxStyle::~TextBoxStyle()
	{}

	void TextBoxStyle::SetShowBorder(bool show)
	{
		m_showBorder = show;
	}

	void TextBoxStyle::SetBackgroundColor(const Color& color)
	{
		m_backgroundColor = color;
	}

	void TextBoxStyle::SetBorderColor(const Color& color)
	{
		m_borderColor = color;
	}

	void TextBoxStyle::SetBorderSize(int size)
	{
		m_borderSize = size;
	}
}
