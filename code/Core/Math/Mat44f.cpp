/********************************************************************************/
/* Copyright (C) 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Core/Math/Mat44f.h"
#include "Core/Math/Constants.h"
#include "Core/Math/Quaternion.h"
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

	Quaternion Mat44f::GetRotationQuaternion() const
	{
		DirectX::XMVECTOR dxQuat = DirectX::XMQuaternionRotationMatrix(m_matrix);
		return Quaternion(dxQuat.m128_f32[0], dxQuat.m128_f32[1], dxQuat.m128_f32[2], dxQuat.m128_f32[3]);
	}

	void Mat44f::Decompose(Core::Vec4f& t, Core::Vec4f& r, Core::Vec4f& s) const
	{
		t = GetT();

		float sx = GetX().Length();
		float sy = GetY().Length();
		float sz = GetZ().Length();

		s = Core::Vec4f(sx, sy, sz, 0);

		//Extract the rotation matrix
		Core::Vec4f R0 = GetX() * (1.f / sx);
		Core::Vec4f R1 = GetY() * (1.f / sy);
		Core::Vec4f R2 = GetZ() * (1.f / sz);

		//this matrix R = Rz * Ry * Rx
		//Rx =|	1	0		0	 |
		//	  |	0	cosX	sinX |
		//	  |	0	-sinX	cosX |
		//
		//Ry =|	cosy	0	-siny |
		//	  |	0		1	0	  |
		//	  |	siny	0	cosy  |
		//
		//Rz =|	cosz  sinz	0	  |
		//	  |	-sinz cosz	0	  |
		//	  |	0		0	1	  |
		//
		// so Rz * Ry = | cosz * cosy	sinz	-cosz * siny |
		//				| -sinz * cosy	cosz	sinz * siny	 |
		//				| siny			0		cosy		 |
		// 
		// so R =	| cosz * cosy		sinz * cosx + cosz * siny * sinx	sinz * sinx - cosz * siny * cosx |
		//			| -sinz * cosy		cosz * cosx - sinz * siny * sinx	cosz * sinx + sinz * siny * cosx |
		//			| siny				-cosy * sinx						cosy * cosx						 |
		// 
		// from m00 amd m10 we can get z using atan2. Both values are scaled by cosy so it works.
		// from m21 and m23 we can get x using atan2. Both values are scaled by cosy so it works.
		// from m20 I can get y.

		float x, y, z = 0;

		if (R2.GetX() > 0.9999999)
		{
			//y is 90 degrees, that's gimbal lock. There is an infinite number of solution.
			//let's set z to 0.
			// Now if y = 90 then siny = 1, cosy = 0
			// then if z = 0 then sinz = 0, cosz = 1
			//so when putting this into R, R is simplified to:
			// R =	| 0		sinX	-cosX |
			//		| 0		cosX	sinX |
			//		| 1		0		0	 |	
			// so x = atan2(sinX, cosX)

			x = std::atan2(R0.GetY(), -R0.GetZ());
			y = Core::PI_OVER_TWO;
		}
		else if (R2.GetX() < -0.9999999)
		{
			//y is -90 degrees, that's gimbal lock. There is an infinite number of solution.
			//let's set z to 0.
			// Now if y = -90 then siny = -1, cosy = 0
			// then if z = 0 then sinz = 0, cosz = 1
			//so when putting this into R, R is simplified to:
			// R =	| 0		-sinX	cosX |
			//		| 0		cosX	-cosX|
			//		| 1		0		0	 |	
			// so x = atan2(sinX, cosX)

			x = std::atan2(-R0.GetY(), R0.GetZ());
			y = -Core::PI_OVER_TWO;
		}
		else
		{
			x = std::atan2(-R2.GetY(), R2.GetZ());
			y = std::asin(R2.GetX());
			z = std::atan2(-R1.GetX(), R0.GetX());
		}

		r.Set(0, x);
		r.Set(1, y);
		r.Set(2, z);
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

	Mat44f Mat44f::CreateRotationMatrixFromQuaternion(const Quaternion& quat)
	{
		Mat44f rotationMatrix;
		rotationMatrix.m_matrix = DirectX::XMMatrixRotationQuaternion(quat.m_data);
		return rotationMatrix;
	}

	Mat44f Mat44f::CreateRotationX(float angle)
	{
		DirectX::XMMATRIX m = DirectX::XMMatrixRotationX(angle);
		Mat44f ret;
		ret.m_matrix = m;
		return ret;
	}

	Mat44f Mat44f::CreateRotationY(float angle)
	{
		DirectX::XMMATRIX m = DirectX::XMMatrixRotationY(angle);
		Mat44f ret;
		ret.m_matrix = m;
		return ret;
	}

	Mat44f Mat44f::CreateRotationZ(float angle)
	{
		DirectX::XMMATRIX m = DirectX::XMMatrixRotationZ(angle);
		Mat44f ret;
		ret.m_matrix = m;
		return ret;
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
		Mat44f m = CreateRotationMatrixFromQuaternion(sqt.GetRotationQuaternion());
		m.SetRow(0, m.GetX() * sqt.GetScale().GetX());
		m.SetRow(1, m.GetY() * sqt.GetScale().GetY());
		m.SetRow(2, m.GetZ() * sqt.GetScale().GetZ());

		m.SetRow(3, sqt.GetTranslation());

		return m;
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
