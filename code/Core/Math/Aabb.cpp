/********************************************************************/
/* © 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Core/Math/Aabb.h"

namespace Core
{
	Aabb::Aabb()
		: m_min()
		, m_max()
	{}

	Aabb::Aabb(const Vec4f& min, const Vec4f& max)
		: m_min(min)
		, m_max(max)
	{}

	const Vec4f& Aabb::GetMin() const
	{
		return m_min;
	}

	const Vec4f& Aabb::GetMax() const
	{
		return m_max;
	}
}
