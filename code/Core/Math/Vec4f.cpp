/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Core/Math/Vec4f.h"

namespace Core
{
	Vec4f::Vec4f()
		: m_values()
	{}

	Vec4f::Vec4f(float x, float y, float z, float w)
	{
		m_values[0] = x;
		m_values[1] = y;
		m_values[2] = z;
		m_values[3] = w;
	}

	float Vec4f::GetX() const { return m_values[0]; }
	float Vec4f::GetY() const { return m_values[1]; }
	float Vec4f::GetZ() const { return m_values[2]; }
	float Vec4f::GetW() const { return m_values[3]; }

	float Vec4f::Get(int index) const
	{
		return m_values[index];
	}

	void Vec4f::Set(int index, float f)
	{
		m_values[index] = f;
	}

	float Vec4f::Dot(const Vec4f& other)
	{
		return GetX() * other.GetX() + GetY() * other.GetY() + GetZ() * other.GetZ();
	}
}
