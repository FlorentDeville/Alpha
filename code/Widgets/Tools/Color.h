/********************************************************************/
/* © 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

namespace Widgets
{
	class Color
	{
	public:
		Color(int r, int g, int b, int a = 255);
		Color(float r, float g, float b, float a = 1.f);
		~Color();

		float m_channels[4];
	};
}
