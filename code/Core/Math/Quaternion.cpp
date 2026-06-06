/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Core/Math/Quaternion.h"

#include "Core/Math/Mat44f.h"
#include "Core/Math/Vec4f.h"

#include <cmath>

namespace Core
{
	Quaternion::Quaternion()
		: Quaternion(0, 0, 0, 1)
	{ }

	Quaternion::Quaternion(float x, float y, float z, float w)
	{
		m_data.m128_f32[0] = x;
		m_data.m128_f32[1] = y;
		m_data.m128_f32[2] = z;
		m_data.m128_f32[3] = w;
	}

	void Quaternion::Normalize()
	{
		//[xx, yy, zz, ww]
		__m128 sq = _mm_mul_ps(m_data, m_data);

		//[y, x, w, z]
		__m128 v1 = _mm_shuffle_ps(sq, sq, _MM_SHUFFLE(1, 0, 3, 2));

		//[y+x, x+y, w+z, z+w]
		sq = _mm_add_ps(sq, v1);

		//[w+z, z+w, y+x, x+y]
		v1 = _mm_shuffle_ps(sq, sq, _MM_SHUFFLE(2, 3, 0, 1));

		//[y+x+w+z, z+w+x+y, y+x+w+z, x+y+z+w]
		sq = _mm_add_ps(sq, v1);

		// inverse square root
		__m128 invSqrt = _mm_rsqrt_ps(sq);

		m_data = _mm_mul_ps(m_data, invSqrt);
	}

	Core::Vec4f Quaternion::ToEulerAngles() const
	{
		Core::Mat44f m = Core::Mat44f::CreateRotationMatrixFromQuaternion(*this);
		
		Core::Vec4f t, r, s;
		m.Decompose(t, r, s);
		
		return r;
	}

	Quaternion Quaternion::FromEulerAngles(const Core::Vec4f& eulerAngles)
	{
		Core::Vec4f half = eulerAngles * 0.5f;
		float cosX = cos(half.GetX());
		float cosY = cos(half.GetY());
		float cosZ = cos(half.GetZ());

		float sinX = sin(half.GetX());
		float sinY = sin(half.GetY());
		float sinZ = sin(half.GetZ());

		float w = cosX * cosY * cosZ + sinX * sinY * sinZ;
		float x = sinX * cosY * cosZ - cosX * sinY * sinZ;
		float y = cosX * sinY * cosZ + sinX * cosY * sinZ;
		float z = cosX * cosY * sinZ - sinX * sinY * cosZ;
		return Quaternion(x, y, z, w);
	}

	Quaternion Quaternion::FromEulerAngles(float x, float y, float z)
	{
		return FromEulerAngles(Core::Vec4f(x, y, z, 0));
	}

	Quaternion Quaternion::FromAxisAngle(const Core::Vec4f& axis, float angle)
	{
		float halfAngle = angle * 0.5f;

		float cosAngle = cos(halfAngle);
		float sinAngle = sin(halfAngle);

		Core::Vec4f tempAxis = axis;
		tempAxis.Set(3, 1);

		Core::Vec4f cossin(sinAngle, sinAngle, sinAngle, cosAngle);

		Core::Vec4f result = tempAxis * cossin;
		return Quaternion(result.GetX(), result.GetY(), result.GetZ(), result.GetW());
	}

	bool Quaternion::operator==(const Quaternion& other) const
	{
		__m128 res = _mm_cmpeq_ps(m_data, other.m_data);

		// Extract the most significant bit of each 32-bit float into an integer mask
		int mask = _mm_movemask_ps(res);

		// Check if all bits in the mask are set (i.e., equal to 0xF or 15)
		return mask == 0xF;
	}

	Quaternion Quaternion::operator*(const Quaternion& other) const
	{
		//for quaternion expressed as a scalar and a vector : q1 = (s1, v1) and q2 = (s2, v2)
		// q1 * q2 = (s1 * s2 - v1 . v2, s1v2 + s2v1 + v1 X v2)

		Core::Vec4f v1(m_data);
		Core::Vec4f v2(other.m_data);
		
		float scalarPart = (GetW() * other.GetW()) - (v1.Dot(v2));
		Core::Vec4f vectorPart = v2 * GetW() + v1 * other.GetW() + v1.Cross(v2);

		return Quaternion(vectorPart.GetX(), vectorPart.GetY(), vectorPart.GetZ(), scalarPart);
	}

	float Quaternion::GetX() const
	{
		return m_data.m128_f32[0];
	}

	float Quaternion::GetY() const
	{
		return m_data.m128_f32[1];
	}

	float Quaternion::GetZ() const
	{
		return m_data.m128_f32[2];
	}

	float Quaternion::GetW() const
	{
		return m_data.m128_f32[3];
	}
}
