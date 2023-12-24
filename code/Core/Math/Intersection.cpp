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
}