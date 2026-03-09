/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Rendering/Mesh/Mesh.h"

#include "Systems/Objects/AssetObject.h"
#include "Core/Reflection/ReflectionMacro.h"

ENABLE_REFLECTION(Systems, MeshAsset)

namespace Systems
{
	class MeshAsset : public AssetObject
	{
	public:
		MeshAsset();
		~MeshAsset();

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

		START_REFLECTION(Systems::MeshAsset)
			ADD_BASETYPE(Systems::AssetObject)
			ADD_FIELD(m_sourceFile)
			ADD_FIELD_ATTR(m_position, Core::Hidden)
			ADD_FIELD_ATTR(m_uv, Core::Hidden)
			ADD_FIELD_ATTR(m_color, Core::Hidden)
			ADD_FIELD_ATTR(m_normal, Core::Hidden)
			ADD_FIELD_ATTR(m_indices, Core::Hidden)
		END_REFLECTION()
	};
}
