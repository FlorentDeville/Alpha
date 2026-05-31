/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Core/Collections/Array.h"
#include "Core/Math/Vec4f.h"
#include "Systems/Game/Subsystems/ISubsystem.h"

namespace Systems
{
	class NavmeshSubsystem : public ISubsystem
	{
	public:
		NavmeshSubsystem();
		~NavmeshSubsystem();

		void AddQuad(const Core::Vec4f (&points)[4]);

		Core::Vec4f QueryRandomPosition() const;

		static uint32_t m_subsystemIndex;
		static NavmeshSubsystem* GetSubsystem();

	private:

		struct Triangle
		{
			Core::Vec4f m_points[3];
		};

		//for now a navmesh is a collection of triangles
		Core::Array<Triangle> m_navmesh;

		float ComputeTriangleArea(const Core::Vec4f& p1, const Core::Vec4f& p2, const Core::Vec4f& p3) const;
	};
}
