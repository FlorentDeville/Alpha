/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Core/Math/Vec4f.h"

#include "Rendering/Mesh/Mesh.h"

#include "Systems/Objects/AssetObject.h"
#include "Systems/Reflection/ReflectionMacro.h"

namespace Systems
{
	ENABLE_REFLECTION_WITH_NS(Systems, MeshAsset)
	class MeshAsset : public AssetObject
	{
	public:
		MeshAsset();
		~MeshAsset();

		void Init(const std::string& sourceFile, std::vector<Core::Vec4f>&& pos, std::vector<Core::Vec4f>&& uv, std::vector<Core::Vec4f>&& color,
			std::vector<uint16_t>&& indices);

		const Rendering::Mesh* GetRenderingMesh() const;

	private:

		std::string m_sourceFile;

		//All those arrays are the same size. Each element represents an information for a single vertex
		std::vector<Core::Vec4f> m_position;	//should be a vec3
		std::vector<Core::Vec4f> m_uv;			//should be a vec2
		std::vector<Core::Vec4f> m_color;

		//Each 3 consecutive indices makes a triangle
		std::vector<uint16_t> m_indices;

		//The low level mesh used by the renderer
		Rendering::Mesh m_mesh;

		START_REFLECTION(Systems::MeshAsset)
			ADD_BASETYPE(Systems::AssetObject)
			ADD_FIELD(m_sourceFile)
			ADD_FIELD(m_position)
			ADD_FIELD(m_uv)
			ADD_FIELD(m_color)
			ADD_FIELD(m_indices)
		END_REFLECTION()
	};
}
