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

		static bool RayVsPlane(const Ray& ray, const Vec4f& planeNormal, const Vec4f& planePoint, float& t);

		static bool RayVsDisk(const Ray& ray, const Vec4f& diskNormal, const Vec4f& diskCenter, float innerRadius, float outerRadius, float& t);

		static void RayVsRay_ClosestPoint(const Ray& rayA, const Ray& rayB, Vec4f& closestPointRayB);

		static void RayVsCircle_ClosestPoint(const Ray& ray, const Vec4f& circleNormal, const Vec4f& circleCenter, float radius, Vec4f& closestPoint);
	};
}
