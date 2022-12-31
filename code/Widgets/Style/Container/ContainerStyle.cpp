/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Widgets/Style/Container/ContainerStyle.h"

namespace Widgets
{
	ContainerStyle::ContainerStyle()
		: BaseStyle()
		, m_backgroundColor(DirectX::XMVectorSet(0.12f, 0.12f, 0.12f, 1.f))
		, m_borderColor(DirectX::XMVectorSet(1.f, 1.f, 1.f, 1.f))
		, m_showBorder(false)
		, m_borderSize(1)
		, m_padding()
	{}

	ContainerStyle::~ContainerStyle()
	{}

	void ContainerStyle::SetBorderSize(int size)
	{
		m_borderSize = size;
	}

	void ContainerStyle::SetBackgroundColor(const DirectX::XMVECTOR& backgroundColor)
	{
		m_backgroundColor = backgroundColor;
	}

	void ContainerStyle::SetBorderColor(const DirectX::XMVECTOR& borderColor)
	{
		m_borderColor = borderColor;
	}

	void ContainerStyle::ShowBorder(bool showBorder)
	{
		m_showBorder = showBorder;
	}
}
