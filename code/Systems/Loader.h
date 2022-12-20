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
	class LoaderParameter
	{
	public:
		std::string m_dataMeshPath;
		std::string m_dataMaterialPath;
		std::string m_dataShaderPath;
		std::string m_dataTexturePath;
	};

	class Loader : public Core::Singleton<Loader>
	{
	public:
		Loader();
		~Loader();

		bool Init(const LoaderParameter& parameter);

		bool LoadMesh(const std::string& absFilename, Rendering::Mesh& mesh);
		bool LoadMaterial(const std::string& absFilename, Rendering::Material& material);

	private:
		std::string m_dataMeshPath;
		std::string m_dataMaterialPath;
		std::string m_dataShaderPath;
		std::string m_dataTexturePath;
	};
}
