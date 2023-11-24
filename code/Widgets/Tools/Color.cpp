/********************************************************************/
/* © 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Widgets/Tools/Color.h"

namespace Widgets
{
	Color::Color(int r, int g, int b, int a)
	{
		m_channels[0] = r / 255.f;
		m_channels[1] = g / 255.f;
		m_channels[2] = b / 255.f;
		m_channels[3] = a / 255.f;
	}

	Color::Color(float r, float g, float b, float a)
	{
		m_channels[0] = r;
		m_channels[1] = g;
		m_channels[2] = b;
		m_channels[3] = a;
	}

	Color::~Color()
	{}
}
