/********************************************************************/
/* © 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Core/Math/Ray.h"

namespace Core
{
	Ray::Ray()
		: m_origin()
		, m_direction()
	{}

	Ray::Ray(const Vec4f& origin, const Vec4f& direction)
		: m_origin(origin)
		, m_direction(direction)
	{
		m_origin.Set(3, 1);
		m_direction.Set(3, 0);
	}

	void Ray::Transform(const Mat44f& transform)
	{
		m_origin = m_origin * transform;
		m_direction = m_direction * transform;
	}

	Vec4f Ray::ComputeInverseDirection() const
	{
		return Core::Vec4f(1.f / m_direction.GetX(), 1.f / m_direction.GetY(), 1.f / m_direction.GetZ(), 0);
	}

	const Vec4f& Ray::GetOrigin() const
	{
		return m_origin;
	}

	const Vec4f& Ray::GetDirection() const
	{
		return m_direction;
	}
}
