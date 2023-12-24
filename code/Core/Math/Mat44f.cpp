/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Core/Math/Mat44f.h"

#include "Core/Math/Vec4f.h"

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

	Mat44f::Mat44f(const DirectX::XMMATRIX& matrix)
		: m_matrix(matrix)
	{}
}
