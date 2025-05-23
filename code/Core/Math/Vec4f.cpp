/********************************************************************/
/* � 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Core/Math/Vec4f.h"

#include "Core/Math/Mat44f.h"

#include <cmath>

using namespace DirectX;

namespace Core
{
	Vec4f::Vec4f()
		: m_vector()
	{}

	Vec4f::Vec4f(float x, float y, float z, float w)
	{
		m_vector = DirectX::XMVectorSet(x, y, z, w);
	}

	float Vec4f::GetX() const { return m_vector.m128_f32[0]; }
	float Vec4f::GetY() const { return m_vector.m128_f32[1]; }
	float Vec4f::GetZ() const { return m_vector.m128_f32[2]; }
	float Vec4f::GetW() const { return m_vector.m128_f32[3]; }

	float Vec4f::Get(int index) const
	{
		return m_vector.m128_f32[index];
	}

	void Vec4f::Set(int index, float f)
	{
		m_vector.m128_f32[index] = f;
	}

	float Vec4f::Dot(const Vec4f& other) const
	{
		DirectX::XMVECTOR res = DirectX::XMVector3Dot(m_vector, other.m_vector);
		return res.m128_f32[0];
	}

	float Vec4f::Dot4(const Vec4f& other) const
	{
		DirectX::XMVECTOR res = DirectX::XMVector4Dot(m_vector, other.m_vector);
		return res.m128_f32[0];
	}

	Vec4f Vec4f::Cross(const Vec4f& other) const
	{
		DirectX::XMVECTOR res = DirectX::XMVector3Cross(m_vector, other.m_vector);
		return Vec4f(res);
	}

	float Vec4f::Length() const
	{
		float squaredLength = Dot(*this);
		return sqrt(squaredLength);
	}

	void Vec4f::Normalize()
	{
		m_vector = DirectX::XMVector3Normalize(m_vector);
	}

	Vec4f Vec4f::operator+(const Vec4f& other) const
	{
		DirectX::XMVECTOR dxRes = DirectX::XMVectorAdd(m_vector, other.m_vector);
		return Vec4f(dxRes);
	}

	Vec4f Vec4f::operator-(const Vec4f& other) const
	{
		DirectX::XMVECTOR dxRes = DirectX::XMVectorSubtract(m_vector, other.m_vector);
		return Vec4f(dxRes);
	}

	Vec4f Vec4f::operator*(const Mat44f& other) const
	{
		return Vec4f(DirectX::XMVector4Transform(m_vector, other.m_matrix));
	}

	Vec4f Vec4f::operator*(float f) const
	{
		return Vec4f(f * m_vector);
	}

	Vec4f Vec4f::QuaternionToEulerAngles(const Vec4f& quaternion)
	{
		float x = quaternion.GetX();
		float y = quaternion.GetY();
		float z = quaternion.GetZ();
		float w = quaternion.GetW();

		// roll (x-axis rotation)
		float sinr_cosp = 2 * (w * x + y * z);
		float cosr_cosp = 1 - 2 * (x * x + y * y);
		float roll = std::atan2(sinr_cosp, cosr_cosp);

		// pitch (y-axis rotation)
		float sinp = std::sqrt(1 + 2 * (w * y - x * z));
		float cosp = std::sqrt(1 - 2 * (w * y - x * z));
		float pitch = 2 * std::atan2(sinp, cosp) - DirectX::XM_PIDIV2;

		// yaw (z-axis rotation)
		float siny_cosp = 2 * (w * z + x * y);
		float cosy_cosp = 1 - 2 * (y * y + z * z);
		float yaw = std::atan2(siny_cosp, cosy_cosp);

		return Vec4f(roll, pitch, yaw, 0);
	}

	Vec4f::Vec4f(const DirectX::XMVECTOR& vector)
		: m_vector(vector)
	{}
}
