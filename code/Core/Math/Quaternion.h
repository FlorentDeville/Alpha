/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include <xmmintrin.h>

namespace Core
{
	class Vec4f;

	class Quaternion
	{
	public:
		Quaternion();
		Quaternion(float x, float y, float z, float w);
		~Quaternion() = default;

		void Normalize();
		
		Core::Vec4f ToEulerAngles() const;
		Core::Vec4f ToVec4f() const;

		static Quaternion FromEulerAngles(const Core::Vec4f& eulerAngles);
		static Quaternion FromEulerAngles(float x, float y, float z);
		static Quaternion FromAxisAngle(const Core::Vec4f& axis, float angle);

		static Quaternion Slerp(const Core::Quaternion& q0, const Core::Quaternion& q1, float t);

		bool operator==(const Quaternion& other) const;
		Quaternion operator*(const Quaternion& other) const;
		Quaternion operator+(const Quaternion& other) const;
		Quaternion operator*(float f) const;

		float GetX() const;
		float GetY() const;
		float GetZ() const;
		float GetW() const;

		Quaternion Conjugate() const;

		__m128 m_data;
	};
}
