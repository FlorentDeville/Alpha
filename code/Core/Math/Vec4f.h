/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include <DirectXMath.h>

namespace Core
{
	class Mat44f;

	class Vec4f
	{
		friend Mat44f;

	public:
		Vec4f();
		Vec4f(float x, float y, float z, float w);

		float GetX() const;
		float GetY() const;
		float GetZ() const;
		float GetW() const;

		float Get(int index) const;

		void Set(int index, float f);

		float Dot(const Vec4f& other) const;
		float Dot4(const Vec4f& other) const;

		float Length() const;

		void Normalize();

		Vec4f operator-(const Vec4f& other) const;

	private:
		explicit Vec4f(const DirectX::XMVECTOR& vector);

		DirectX::XMVECTOR m_vector;
	};
}
