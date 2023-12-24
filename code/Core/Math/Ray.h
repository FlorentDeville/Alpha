/********************************************************************/
/* © 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Core/Math/Vec4f.h"

namespace Core
{
	class Ray
	{
	public:
		Ray();
		Ray(const Vec4f& origin, const Vec4f& direction);

		void Transform(const Mat44f& transform);

		Vec4f ComputeInverseDirection() const;

		const Vec4f& GetOrigin() const;
		const Vec4f& GetDirection() const;

	private:
		Vec4f m_origin;
		Vec4f m_direction;
	};
}
