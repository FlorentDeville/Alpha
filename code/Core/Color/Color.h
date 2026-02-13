/********************************************************************/
/* © 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

namespace Core
{
	class Color
	{
	public:
		Color();
		Color(float r, float g, float b);

		float GetRed() const;
		float GetGreen() const;
		float GetBlue() const;

		void SetRed(float r);
		void SetGreen(float g);
		void SetBlue(float b);

	private:
		float m_r;
		float m_g;
		float m_b;
	};
}
