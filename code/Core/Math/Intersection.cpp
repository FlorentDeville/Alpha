/********************************************************************/
/* © 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Core/Math/Intersection.h"

#include "Core/Math/Aabb.h"
#include "Core/Math/Ray.h"

#include <algorithm>

namespace Core
{
	bool Intersection::RayVsAabb(const Ray& ray, const Aabb& aabb)
	{
		float tmin = 0;
		float tmax = FLT_MAX;
		Core::Vec4f invRayDirectionLs = ray.ComputeInverseDirection();
		for (int ii = 0; ii < 3; ++ii)
		{
			float t1 = (aabb.GetMin().Get(ii) - ray.GetOrigin().Get(ii)) * invRayDirectionLs.Get(ii);
			float t2 = (aabb.GetMax().Get(ii) - ray.GetOrigin().Get(ii)) * invRayDirectionLs.Get(ii);

			tmin = std::min(std::max(t1, tmin), std::max(t2, tmin));
			tmax = std::max(std::min(t1, tmax), std::min(t2, tmax));
		}

		if (tmin < tmax)
			return true;

		return false;
	}
}