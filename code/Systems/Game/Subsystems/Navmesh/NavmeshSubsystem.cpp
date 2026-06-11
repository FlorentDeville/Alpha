/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Systems/Game/Subsystems/Navmesh/NavmeshSubsystem.h"

#include "Core/Random/Random.h"

#include "Systems/Game/GameMgr.h"

namespace Systems
{
	uint32_t NavmeshSubsystem::m_subsystemIndex = UINT32_MAX;

	NavmeshSubsystem::NavmeshSubsystem()
		: ISubsystem()
	{ }

	NavmeshSubsystem::~NavmeshSubsystem()
	{ }

	void NavmeshSubsystem::AddQuad(const Core::Vec4f(&points)[4])
	{
		Triangle& t1 = m_navmesh.PushBackDefault();
		t1.m_points[0] = points[0];
		t1.m_points[1] = points[1];
		t1.m_points[2] = points[2];

		Triangle& t2 = m_navmesh.PushBackDefault();
		t2.m_points[0] = points[0];
		t2.m_points[1] = points[2];
		t2.m_points[2] = points[3];
	}

	void NavmeshSubsystem::ClearAllQuads()
	{
		m_navmesh.Clear();
	}

	Core::Vec4f NavmeshSubsystem::QueryRandomPosition() const
	{
		float area1 = ComputeTriangleArea(m_navmesh[0].m_points[0], m_navmesh[0].m_points[1], m_navmesh[0].m_points[2]);
		float area2 = ComputeTriangleArea(m_navmesh[1].m_points[0], m_navmesh[1].m_points[1], m_navmesh[1].m_points[2]);
		float totalArea = area1 + area2;

		Core::RandomFloat floatGenerator(0, totalArea);
		float triangleChoice = floatGenerator.Generate();
		
		uint32_t triangleIndex = 0;
		if (triangleChoice > area1) 
			triangleIndex = 1;

		Core::RandomFloat randomParameter(0, 1);
		float u = randomParameter.Generate();
		float v = randomParameter.Generate();

		if (u + v > 1)
		{
			u = 1 - u;
			v = 1 - v;
		}
		
		const Triangle& tri = m_navmesh[triangleIndex];
		Core::Vec4f p = tri.m_points[0] + (tri.m_points[1] - tri.m_points[0]) * u + (tri.m_points[2] - tri.m_points[0]) * v;
		return p;
	}

	NavmeshSubsystem* NavmeshSubsystem::GetSubsystem()
	{
		return static_cast<NavmeshSubsystem*>(GameMgr::Get().GetGameSubsystem(m_subsystemIndex));
	}

	float NavmeshSubsystem::ComputeTriangleArea(const Core::Vec4f& p1, const Core::Vec4f& p2, const Core::Vec4f& p3) const
	{
		Core::Vec4f v1 = p2 - p1;
		Core::Vec4f v2 = p3 - p1;

		Core::Vec4f cross = v1.Cross(v2);

		float area = cross.Length() * 0.5f;
		return area;
	}
}
