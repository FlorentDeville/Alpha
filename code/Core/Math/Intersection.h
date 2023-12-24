/********************************************************************/
/* © 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

namespace Core
{
	class Aabb;
	class Ray;

	class Intersection
	{
	public:
		static bool RayVsAabb(const Ray& ray, const Aabb& aabb);
	};
}
