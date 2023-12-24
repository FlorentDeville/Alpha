/********************************************************************/
/* © 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

namespace Core
{
	class Aabb;
	class Ray;
	class Vec4f;

	class Intersection
	{
	public:
		static bool RayVsAabb(const Ray& ray, const Aabb& aabb);

		static void RayVsRay_ClosestPoint(const Ray& rayA, const Ray& rayB, Vec4f& closestPointRayB);
	};
}
