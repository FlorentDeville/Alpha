/********************************************************************/
/* � 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Core/Collections/Array.h"
#include "Core/Math/Vec4f.h"

#include "Rendering/Mesh/Mesh.h"

#include "Systems/Assets/AssetObjects/Mesh/MeshAssetUpgrade.h"
#include "Systems/Objects/AssetObject.h"
#include "Systems/Reflection/ReflectionMacro.h"

namespace Systems
{
	ENABLE_REFLECTION_WITH_NS(Systems, MeshAsset_v1)
	class MeshAsset_v1 : public AssetObject
	{
	public:
		MeshAsset_v1();
		~MeshAsset_v1();

		void Init(const std::string& sourceFile, Core::Array<Core::Vec4f>& pos, Core::Array<Core::Vec4f>& uv, Core::Array<Core::Vec4f>& color,
			Core::Array<uint16_t>& indices);

		const std::string& GetSourceFile() const;

		const Rendering::Mesh* GetRenderingMesh() const;

		void PostLoad() override;

		const Core::Array<Core::Vec4f>& GetPositions() const;
		const Core::Array<Core::Vec4f>& GetUvs() const;
		const Core::Array<Core::Vec4f>& GetColors() const;
		const Core::Array<uint16_t>& GetIndices() const;

	private:

		std::string m_sourceFile;

		//All those arrays are the same size. Each element represents an information for a single vertex
		Core::Array<Core::Vec4f> m_position;	//should be a vec3
		Core::Array<Core::Vec4f> m_uv;			//should be a vec2
		Core::Array<Core::Vec4f> m_color;

		//Each 3 consecutive indices makes a triangle
		Core::Array<uint16_t> m_indices;

		//The low level mesh used by the renderer
		Rendering::Mesh m_mesh;

		START_REFLECTION(Systems::MeshAsset_v1)
			ADD_BASETYPE(Systems::AssetObject)
			ADD_FIELD(m_sourceFile)
			ADD_FIELD(m_position)
			ADD_FIELD(m_uv)
			ADD_FIELD(m_color)
			ADD_FIELD(m_indices)
			ADD_UPGRADE_TYPE(Systems::MeshAsset_v2, UpgradeMeshAssetV1ToV2)
		END_REFLECTION()
	};
}
