/********************************************************************************/
/* Copyright (C) 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include <string>

#include "Core/Collections/Array.h"
#include "Core/Math/Vec4f.h"

namespace ofbx
{
	struct Mesh;
	struct Object;
}

namespace Systems
{
	class MeshAsset;
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

		Core::Array<float> m_position;
		Core::Array<float> m_uv;
		Core::Array<float> m_color;
		Core::Array<float> m_normal;

		Core::Array<uint16_t> m_indices;
	};
}
