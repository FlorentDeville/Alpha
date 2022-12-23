/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

namespace Core
{
	class Vec4f
	{
	public:
		Vec4f();
		Vec4f(float x, float y, float z, float w);

		float GetX() const;
		float GetY() const;
		float GetZ() const;
		float GetW() const;

	private:
		float m_x;
		float m_y;
		float m_z;
		float m_w; //0 for a vector, 1 for a point
	};
}
