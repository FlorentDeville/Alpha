/********************************************************************/
/* © 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

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
	};
}
