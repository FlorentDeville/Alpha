/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Rendering/Mesh/Mesh.h"

#include "Systems/Objects/AssetObject.h"
#include "Systems/Reflection/ReflectionMacro.h"

namespace Systems
{
	ENABLE_REFLECTION_WITH_NS(Systems, MeshAsset_v2)
	class MeshAsset_v2 : public AssetObject
	{
	public:
		MeshAsset_v2();
		~MeshAsset_v2();

		void Init(const std::string& sourceFile, Core::Array<float>&& pos, Core::Array<float>&& uv, Core::Array<float>&& color, Core::Array<float>&& normals, Core::Array<uint16_t>&& indices);

		const std::string& GetSourceFile() const;

		const Rendering::Mesh* GetRenderingMesh() const;

		void PostLoad() override;

		static const std::string& GetAssetTypeName();
		static Core::Sid GetAssetTypeNameSid();

	private:

		std::string m_sourceFile;

		//All those arrays are the same size. Each element represents an information for a single vertex
		Core::Array<float> m_position;	// every 3 float is a position
		Core::Array<float> m_uv;		// every 2 float is a uv
		Core::Array<float> m_color;		// every 4 float is a color
		Core::Array<float> m_normal;	// every 3 float is a normal

		//Each 3 consecutive indices makes a triangle
		Core::Array<uint16_t> m_indices;

		//The low level mesh used by the renderer
		Rendering::Mesh m_mesh;

		START_REFLECTION(Systems::MeshAsset_v2)
			ADD_BASETYPE(Systems::AssetObject)
			ADD_FIELD(m_sourceFile)
			ADD_FIELD(m_position)
			ADD_FIELD(m_uv)
			ADD_FIELD(m_color)
			ADD_FIELD(m_normal)
			ADD_FIELD(m_indices)
		END_REFLECTION()
	};
}
