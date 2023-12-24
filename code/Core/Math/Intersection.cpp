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

	void Intersection::RayVsRay_ClosestPoint(const Ray& rayA, const Ray& rayB, Vec4f& closestPointRayB)
	{
		const Core::Vec4f& A = rayA.GetOrigin();
		const Core::Vec4f& aDir = rayA.GetDirection();

		const Core::Vec4f& B = rayB.GetOrigin();
		const Core::Vec4f& bDir = rayB.GetDirection();

		Core::Vec4f C = B - A;

		float ADotA = aDir.Dot(aDir);
		float BDotB = bDir.Dot(bDir);
		float ADotB = aDir.Dot(bDir);
		float ADotC = aDir.Dot(C);
		float BDotC = bDir.Dot(C);

		closestPointRayB = B + bDir * ((ADotB * ADotC - BDotC * ADotA) / ((ADotA * BDotB) - (ADotB * ADotB)));
	}

	bool Intersection::RayVsPlane(const Ray& ray, const Vec4f& planeNormal, const Vec4f& planePoint, float& t)
	{
		float denom = planeNormal.Dot(ray.GetDirection());
		if (denom > 1e-6 || denom < -1e-6)
		{
			Core::Vec4f p0l0 = planePoint - ray.GetOrigin();
			t = p0l0.Dot(planeNormal) / denom;
			if (t >= 0)
				return true;
		}

		return false;
	}

	bool Intersection::RayVsDisk(const Ray& ray, const Vec4f& diskNormal, const Vec4f& center, float innerRadius, float outerRadius, float& t)
	{
		bool intersectPlane = RayVsPlane(ray, diskNormal, center, t);
		if (!intersectPlane)
			return false;

		Core::Vec4f planePoint = ray.GetOrigin() + ray.GetDirection() * t;

		float squaredInnerRadius = innerRadius * innerRadius;
		float squaredOuterRadius = outerRadius * outerRadius;

		Core::Vec4f radius = center - planePoint;
		float squaredDistance = radius.Dot(radius);

		if (squaredDistance >= squaredInnerRadius && squaredDistance <= squaredOuterRadius)
			return true;

		return false;
	}
}