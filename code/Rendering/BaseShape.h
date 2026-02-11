/********************************************************************/
/* © 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include <cstdint>

namespace Rendering
{
	class Mesh;

	class BaseShape
	{
	public:
		static void CreateCircle(Mesh* pMesh, int resolution);
		static void CreateCylinder(Mesh* pMesh, float height, float diameter, int tessellation);
		static void CreateCone(Mesh* pMesh, float diameter, float height, size_t tessellation);
		static void CreateTorus(Mesh* pMesh, float diameter, float thickness, int tessellation);
		static void CreateCube(Mesh* pMesh);

		//Generate a mesh of a unit sphere. Stacks is the number of horizontal subdivision. Slices is the number of vertical subdivision.
		static void CreateSphere(Mesh* pMesh, uint32_t stacks, uint32_t slices);
	};
}
