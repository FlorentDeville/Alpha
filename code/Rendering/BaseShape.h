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
		static void CreateCircle(Rendering::Mesh* pMesh, int resolution);
		static void CreateCylinder(Rendering::Mesh* pMesh, float height, float diameter, int tessellation);
		static void CreateCone(Rendering::Mesh* pMesh, float diameter, float height, size_t tessellation);
	};
}
