/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Widgets/Style/TextBox/TextBoxStyle.h"

namespace Widgets
{
	TextBoxStyle::TextBoxStyle()
		: BaseStyle()
		, m_borderSize(1)
		, m_backgroundColor(DirectX::XMVectorSet(0.22f, 0.22f, 0.22f, 1.f))
		, m_borderColor(DirectX::XMVectorSet(0.26f, 0.26f, 0.26f, 1.f))
		, m_textColor(DirectX::XMVectorSet(0.84f, 0.84f, 0.84f, 1.f))
		, m_showBorder(true)
		, m_padding()
	{}

	TextBoxStyle::~TextBoxStyle()
	{}

	void TextBoxStyle::SetShowBorder(bool show)
	{
		m_showBorder = show;
	}

	void TextBoxStyle::SetBackgroundColor(const DirectX::XMVECTOR& color)
	{
		m_backgroundColor = color;
	}

	void TextBoxStyle::SetBorderColor(const DirectX::XMVECTOR& color)
	{
		m_borderColor = color;
	}

	void TextBoxStyle::SetTextColor(const DirectX::XMVECTOR& color)
	{
		m_textColor = color;
	}

	void TextBoxStyle::SetBorderSize(int size)
	{
		m_borderSize = size;
	}
}
