/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Core/Math/Mat44f.h"

#include "Core/Math/Sqt.h"
#include "Core/Math/Vec4f.h"

#include <cmath>

namespace Core
{
	Mat44f Mat44f::s_identity = Mat44f::CreateIdentity();

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

	Vec4f Mat44f::GetRotationQuaternion() const
	{
		DirectX::XMVECTOR dxQuat = DirectX::XMQuaternionRotationMatrix(m_matrix);
		Vec4f quat;
		quat.m_vector = dxQuat;
		return quat;
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

	Mat44f Mat44f::CreateRotationMatrixFromQuaternion(const Vec4f& quat)
	{
		Mat44f rotationMatrix;
		rotationMatrix.m_matrix = DirectX::XMMatrixRotationQuaternion(quat.m_vector);
		return rotationMatrix;
	}

	Mat44f Mat44f::CreateTranslationMatrix(const Vec4f& translation)
	{
		Mat44f matrix;
		matrix.SetIdentity();
		matrix.SetRow(3, translation);
		matrix.Set(3, 3, 1);
		return matrix;
	}

	Mat44f Mat44f::CreateScaleMatrix(const Vec4f& scale)
	{
		Mat44f matrix;
		matrix.Set(0, 0, scale.GetX());
		matrix.Set(1, 1, scale.GetY());
		matrix.Set(2, 2, scale.GetZ());
		matrix.Set(3, 3, 1);
		return matrix;
	}

	Mat44f Mat44f::CreateTransformMatrix(const Sqt& sqt)
	{
		Mat44f t = CreateTranslationMatrix(sqt.GetTranslation());
		Mat44f q = CreateRotationMatrixFromQuaternion(sqt.GetRotationQuaternion());
		Mat44f s = CreateScaleMatrix(sqt.GetScale());

		Mat44f res = s * q * t;
		return res;
	}

	Mat44f Mat44f::CreateIdentity()
	{
		Mat44f matrix;
		matrix.SetIdentity();
		return matrix;
	}

	Mat44f Mat44f::CreateLookAt(const Core::Vec4f& pos, const Core::Vec4f& dir, const Vec4f& worldUp)
	{
		Core::Vec4f forward = dir;
		forward.Normalize();

		Core::Vec4f localUp = worldUp;
		Core::Vec4f right = worldUp.Cross(forward);
		float rightDot = right.Dot(right);
		if (rightDot == 0)
		{
			localUp = Core::Vec4f(0, 0, 1, 0);
			right = localUp.Cross(forward);
		}

		localUp = forward.Cross(right);

		right.Normalize();
		localUp.Normalize();

		return Core::Mat44f(right, localUp, forward, pos);
	}

	Mat44f Mat44f::CreateView(const Core::Vec4f& pos, const Core::Vec4f& dir, const Vec4f& worldUp)
	{
		Core::Vec4f forward = dir;
		forward.Normalize();

		Core::Vec4f right = worldUp.Cross(forward);
		right.Normalize();

		Core::Vec4f up = forward.Cross(right);

		Core::Vec4f negPos = pos * -1;

		float x = right.Dot(negPos);
		float y = up.Dot(negPos);
		float z = forward.Dot(negPos);
		right.Set(3, x);
		up.Set(3, y);
		forward.Set(3, z);

		Core::Mat44f view(right, up, forward, Core::Vec4f(0, 0, 0, 1));
		view.Transpose();

		return view;
	}

	Mat44f Mat44f::CreateOrthographic(float left, float right, float bottom, float top, float near, float far)
	{
		Core::Mat44f proj;
		proj.m_matrix = DirectX::XMMatrixOrthographicOffCenterLH(left, right, bottom, top, near, far);
		return proj;
	}

	Mat44f Mat44f::CreatePerspective(float fovAngleY, float aspectRatio, float nearZ, float farZ)
	{
		Core::Mat44f proj;
		proj.m_matrix = DirectX::XMMatrixPerspectiveFovLH(fovAngleY, aspectRatio, nearZ, farZ);
		return proj;
	}

	Mat44f::Mat44f(const DirectX::XMMATRIX& matrix)
		: m_matrix(matrix)
	{}
}
