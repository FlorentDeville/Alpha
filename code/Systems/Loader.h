/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Core/Singleton.h"

#include <string>

namespace Rendering
{
	class Material;
	class MaterialId;
	class Mesh;
	class MeshId;
}

namespace Systems
{
	class Loader : public Core::Singleton<Loader>
	{
	public:
		Loader();
		~Loader();

		bool Init(const std::string& dataMeshPath);

		bool LoadMesh(const std::string& name, Rendering::Mesh& mesh);
		bool LoadMaterial(const std::string& name, Rendering::Material& material);

	private:
		std::string m_dataMeshPath;
	};
}
