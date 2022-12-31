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

		float Get(int index) const;

		void Set(int index, float f);

	private:
		float m_values[4];
	};
}
