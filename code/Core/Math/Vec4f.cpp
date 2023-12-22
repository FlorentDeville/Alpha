/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Core/Math/Vec4f.h"

#include <cmath>

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

	float Vec4f::Dot(const Vec4f& other) const
	{
		return GetX() * other.GetX() + GetY() * other.GetY() + GetZ() * other.GetZ();
	}

	float Vec4f::Dot4(const Vec4f& other) const
	{
		return GetX() * other.GetX() + GetY() * other.GetY() + GetZ() * other.GetZ() + GetW() * other.GetW();
	}

	float Vec4f::Length() const
	{
		float squaredLength = Dot(*this);
		return sqrt(squaredLength);
	}

	void Vec4f::Normalize()
	{
		float length = Length();
		float invLength = 1.f / length;

		m_values[0] *= invLength;
		m_values[1] *= invLength;
		m_values[2] *= invLength;
	}

	Vec4f Vec4f::operator-(const Vec4f& other) const
	{
		Vec4f res(
			GetX() - other.GetX(),
			GetY() - other.GetY(),
			GetZ() - other.GetZ(),
			0);

		return res;
	}
}
