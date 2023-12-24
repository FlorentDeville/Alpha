/********************************************************************/
/* © 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Core/Math/Vec4f.h"

namespace Core
{
	class Aabb
	{
	public:
		Aabb();
		Aabb(const Vec4f& min, const Vec4f& max);

		const Vec4f& GetMin() const;
		const Vec4f& GetMax() const;

	private:
		Vec4f m_min;
		Vec4f m_max;
	};
}
