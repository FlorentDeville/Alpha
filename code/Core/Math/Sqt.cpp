/********************************************************************/
/* © 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Core/Math/Sqt.h"

namespace Core
{
	Sqt::Sqt()
		: m_translation(0, 0, 0, 1)
		, m_quat(0, 0, 0, 1)
		, m_scale(1, 1, 1, 0)
		, m_dirty(false)
	{
		m_matrix.SetIdentity();
	}

	Sqt::Sqt(const Mat44f& transform)
		: m_dirty(false)
	{
		m_matrix = transform;
		m_translation = transform.GetT();
		m_scale = Vec4f(transform.GetX().Length(), transform.GetY().Length(), transform.GetZ().Length(), 0);

		Vec4f x = transform.GetX();
		Vec4f y = transform.GetY();
		Vec4f z = transform.GetZ();
		Vec4f t(0, 0, 0, 1);

		x.Normalize();
		y.Normalize();
		z.Normalize();

		Mat44f rotationMatrix(x, y, z, t);
		//m_eulerAngles = rotationMatrix.GetEulerAngle();
		m_quat = rotationMatrix.GetRotationQuaternion();
	}

	void Sqt::SetTranslation(const Vec4f& translation)
	{
		m_translation = translation;
		m_dirty = true;
	}

	void Sqt::SetRotationQuaternion(const Vec4f& quat)
	{
		m_quat = quat;
		m_dirty = true;
	}

	void Sqt::SetScale(const Vec4f& scale)
	{
		m_scale = scale;
		m_dirty = true;
	}

	const Vec4f& Sqt::GetTranslation() const
	{
		return m_translation;
	}

	const Vec4f& Sqt::GetRotationQuaternion() const
	{
		return m_quat;
	}

	const Vec4f& Sqt::GetScale() const
	{
		return m_scale;
	}

	const Mat44f& Sqt::GetMatrix() const
	{
		if (m_dirty)
		{
			m_matrix = Mat44f::CreateTransformMatrix(*this);
			m_dirty = false;
		}

		return m_matrix;
	}
}
