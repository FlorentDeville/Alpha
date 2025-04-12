/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include <string>

#include "Core/Math/Vec4f.h"
#include "Systems/Assets/AssetObjects/MeshAsset.h"

namespace ofbx
{
	struct Mesh;
	struct Object;
}

namespace FbxImporter
{
	class FbxImporter
	{
	public:
		bool Import(const std::string& fbxFilename, Systems::MeshAsset& mesh);

	private:
		bool Visit(const ofbx::Object*);
		bool ImportMesh(const ofbx::Mesh* pMesh);

		std::vector<Core::Vec4f> m_position;
		std::vector<Core::Vec4f> m_uv;
		std::vector<Core::Vec4f> m_color;

		std::vector<uint16_t> m_indices;

		//fbxsdk::FbxManager* m_pManager;
	};
}
