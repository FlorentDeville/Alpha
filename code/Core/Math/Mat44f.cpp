/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Core/Math/Mat44f.h"

#include "Core/Math/Vec4f.h"

#include <cmath>

namespace Core
{
	Mat44f::Mat44f()
		: m_matrix()
	{}

	Mat44f::Mat44f(const Vec4f& x, const Vec4f& y, const Vec4f& z, const Vec4f& pos)
	{
		m_matrix = DirectX::XMMATRIX(x.m_vector, y.m_vector, z.m_vector, pos.m_vector);
	}

	Vec4f Mat44f::GetX() const { return Vec4f(m_matrix.r[0]); }
	Vec4f Mat44f::GetY() const { return Vec4f(m_matrix.r[1]); }
	Vec4f Mat44f::GetZ() const { return Vec4f(m_matrix.r[2]); }
	Vec4f Mat44f::GetT() const { return Vec4f(m_matrix.r[3]); }
	Vec4f Mat44f::GetRow(int row) const { return Vec4f(m_matrix.r[row]); }

	float Mat44f::Get(int row, int column) const
	{
		return m_matrix.r[row].m128_f32[column];
	}

	void Mat44f::Set(int row, int column, float f)
	{
		m_matrix.r[row].m128_f32[column] = f;
	}

	void Mat44f::SetRow(int row, const Vec4f& value)
	{
		m_matrix.r[row] = value.m_vector;
	}

	void Mat44f::SetIdentity()
	{
		m_matrix = DirectX::XMMatrixIdentity();
	}

	void Mat44f::Transpose()
	{
		m_matrix = DirectX::XMMatrixTranspose(m_matrix);
	}

	Mat44f Mat44f::Inverse() const
	{
		return Mat44f(DirectX::XMMatrixInverse(nullptr, m_matrix));
	}

	Mat44f Mat44f::operator*(const Mat44f& other) const
	{
		DirectX::XMMATRIX dxRes = DirectX::XMMatrixMultiply(m_matrix, other.m_matrix);
		return Mat44f(dxRes);
	}

	Vec4f Mat44f::GetEulerAngle() const
	{
		DirectX::XMVECTOR quat = DirectX::XMQuaternionRotationMatrix(m_matrix);

		/*float x = atanf(2 * (quat.m128_f32[0] * quat.m128_f32[1] + quat.m128_f32[2] * quat.m128_f32[3]) / (1 - 2 * (quat.m128_f32[1] * quat.m128_f32[1] + quat.m128_f32[2] * quat.m128_f32[2])));
		float y = asinf(2 * (quat.m128_f32[0] * quat.m128_f32[2] - quat.m128_f32[3] * quat.m128_f32[1]));
		float z = atanf(2 * (quat.m128_f32[0] * quat.m128_f32[3] + quat.m128_f32[1] * quat.m128_f32[2]) / (1 - 2 * (quat.m128_f32[2] * quat.m128_f32[2] + quat.m128_f32[3] * quat.m128_f32[3])));

		return Vec4f(x, y, z, 0);*/

		float x = quat.m128_f32[0];
		float y = quat.m128_f32[1];
		float z = quat.m128_f32[2];
		float w = quat.m128_f32[3];

		// roll (x-axis rotation)
		double sinr_cosp = 2 * (w * x + y * z);
		double cosr_cosp = 1 - 2 * (x * x + y * y);
		float roll = std::atan2(sinr_cosp, cosr_cosp);

		// pitch (y-axis rotation)
		double sinp = std::sqrt(1 + 2 * (w * y - x * z));
		double cosp = std::sqrt(1 - 2 * (w * y - x * z));
		float pitch = 2 * std::atan2(sinp, cosp) - DirectX::XM_PIDIV2;

		// yaw (z-axis rotation)
		double siny_cosp = 2 * (w * z + x * y);
		double cosy_cosp = 1 - 2 * (y * y + z * z);
		float yaw = std::atan2(siny_cosp, cosy_cosp);

		return Vec4f(roll, pitch, yaw, 0);
	}

	Mat44f Mat44f::CreateRotationMatrix(const Vec4f& axis, float angle)
	{
		Mat44f res;
		res.m_matrix = DirectX::XMMatrixRotationAxis(axis.m_vector, angle);
		return res;
	}

	Mat44f Mat44f::CreateRotationMatrixFromEulerAngles(const Vec4f& eulerAngles)
	{
		Mat44f rotationMatrix;
		DirectX::XMMATRIX xRotation = DirectX::XMMatrixRotationX(eulerAngles.GetX());
		DirectX::XMMATRIX yRotation = DirectX::XMMatrixRotationY(eulerAngles.GetY());
		DirectX::XMMATRIX zRotation = DirectX::XMMatrixRotationZ(eulerAngles.GetZ());
		rotationMatrix.m_matrix = zRotation * yRotation * xRotation;
		return rotationMatrix;
	}

	Mat44f::Mat44f(const DirectX::XMMATRIX& matrix)
		: m_matrix(matrix)
	{}
}
