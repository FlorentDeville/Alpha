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

		static Quaternion FromEulerAngles(const Core::Vec4f& eulerAngles);

		bool operator==(const Quaternion& other) const;

		__m128 m_data;
	};
}
