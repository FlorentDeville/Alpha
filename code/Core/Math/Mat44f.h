/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include <DirectXMath.h>

namespace Core
{
	class Sqt;
	class Vec4f;

	class Mat44f
	{
		friend Vec4f;

	public:
		Mat44f();
		Mat44f(const Vec4f& x, const Vec4f& y, const Vec4f& z, const Vec4f& pos);
		explicit Mat44f(const DirectX::XMMATRIX& matrix);

		Vec4f GetX() const;
		Vec4f GetY() const;
		Vec4f GetZ() const;
		Vec4f GetT() const;
		Vec4f GetRow(int row) const;

		float Get(int row, int column) const;

		void Set(int row, int column, float f);
		void SetRow(int row, const Vec4f& value);

		void SetIdentity();

		void Transpose();
		Mat44f Inverse() const;

		Mat44f operator*(const Mat44f& other) const;

		Vec4f GetRotationQuaternion() const;

		static Mat44f CreateRotationMatrix(const Vec4f& axis, float angle);
		static Mat44f CreateRotationMatrixFromEulerAngles(const Vec4f& eulerAngles);
		static Mat44f CreateRotationMatrixFromQuaternion(const Vec4f& quat);
		static Mat44f CreateTranslationMatrix(const Vec4f& translation);
		static Mat44f CreateScaleMatrix(const Vec4f& scale);
		static Mat44f CreateTransformMatrix(const Sqt& sqt);
		static Mat44f CreateIdentity();

		static Mat44f CreateLookAt(const Core::Vec4f& pos, const Core::Vec4f& dir, const Vec4f& up);
		static Mat44f CreateView(const Core::Vec4f& pos, const Core::Vec4f& dir, const Vec4f& up); //Inverse of the LookAt

		static Mat44f CreateOrthographic(float left, float right, float bottom, float top, float nearZ, float farZ);
		static Mat44f CreatePerspective(float fovAngleY, float aspectRatio, float nearZ, float farZ);

		static Mat44f s_identity;

	//private:

		DirectX::XMMATRIX m_matrix;

		
	};
}
