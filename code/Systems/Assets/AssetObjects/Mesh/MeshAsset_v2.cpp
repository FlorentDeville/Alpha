/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Systems/Assets/AssetObjects/Mesh/MeshAsset_v2.h"

namespace Systems
{
	MeshAsset_v2::MeshAsset_v2()
		: AssetObject()
		, m_sourceFile()
		, m_position()
		, m_uv()
		, m_color()
		, m_indices()
		, m_mesh()
	{
	}

	MeshAsset_v2::~MeshAsset_v2()
	{
	}

	void MeshAsset_v2::Init(const std::string& sourceFile,
		Core::Array<float>&& pos,
		Core::Array<float>&& uv,
		Core::Array<float>&& color,
		Core::Array<float>&& normals,
		Core::Array<uint16_t>&& indices)
	{
		m_sourceFile = sourceFile;
		m_position = std::move(pos);
		m_uv = std::move(uv);
		m_color = std::move(color);
		m_normal = std::move(normals);
		m_indices = std::move(indices);

		PostLoad();
	}

	const std::string& MeshAsset_v2::GetSourceFile() const
	{
		return m_sourceFile;
	}

	const Rendering::Mesh* MeshAsset_v2::GetRenderingMesh() const
	{
		return &m_mesh;
	}

	void MeshAsset_v2::PostLoad()
	{
		uint32_t indexCount = m_indices.GetSize();
		uint32_t vertexCount = indexCount;

		std::vector<Rendering::VertexGeneric> vertices;
		vertices.resize(vertexCount);

		for (uint32_t ii = 0; ii < vertexCount; ++ii)
		{
			uint32_t positionIndex = ii * 3;
			uint32_t uvIndex = ii * 2;
			uint32_t colorIndex = ii * 4;
			uint32_t normalIndex = ii * 3;

			Rendering::VertexGeneric& vertex = vertices[ii];

			vertex.Position = DirectX::XMFLOAT3(m_position[positionIndex], m_position[positionIndex + 1], m_position[positionIndex + 2]);
			vertex.Color = DirectX::XMFLOAT3(m_color[colorIndex], m_color[colorIndex + 1], m_color[colorIndex + 2]);
			vertex.Uv = DirectX::XMFLOAT2(m_uv[uvIndex], m_uv[uvIndex + 1]);

			if (m_normal.GetSize() > (normalIndex + 2))
				vertex.Normal = DirectX::XMFLOAT3(m_normal[normalIndex], m_normal[normalIndex + 1], m_normal[normalIndex + 2]);
		}

		m_mesh.LoadVertexAndIndexBuffer(vertices.data(), vertexCount, m_indices.GetData(), indexCount);
	}

	const std::string& MeshAsset_v2::GetAssetTypeName()
	{
		static std::string name = "Mesh";
		return name;
	}

	Core::Sid MeshAsset_v2::GetAssetTypeNameSid()
	{
		static Core::Sid sid = SID(GetAssetTypeName());
		return sid;
	}
}