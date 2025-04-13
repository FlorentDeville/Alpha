/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Systems/Assets/AssetObjects/MeshAsset.h"

namespace Systems
{
	MeshAsset::MeshAsset()
		: AssetObject()
		, m_sourceFile()
		, m_position()
		, m_uv()
		, m_color()
		, m_indices()
		, m_mesh()
	{}

	MeshAsset::~MeshAsset()
	{}

	void MeshAsset::Init(const std::string& sourceFile, Core::Array<Core::Vec4f>& pos, Core::Array<Core::Vec4f>& uv, Core::Array<Core::Vec4f>& color,
		Core::Array<uint16_t>& indices)
	{
		m_sourceFile = sourceFile;
		m_position = std::move(pos);
		m_uv = std::move(uv);
		m_color = std::move(color);
		m_indices = std::move(indices);

		PostLoad();
	}

	const Rendering::Mesh* MeshAsset::GetRenderingMesh() const
	{
		return &m_mesh;
	}

	void MeshAsset::PostLoad()
	{
		int vertexCount = static_cast<int>(m_position.GetSize());
		std::vector<Rendering::VertexGeneric> vertices;
		vertices.resize(vertexCount);

		for (int ii = 0; ii < vertexCount; ++ii)
		{
			const Core::Vec4f& pos = m_position[ii];
			const Core::Vec4f& uv = m_uv[ii];
			const Core::Vec4f& color = m_color[ii];

			Rendering::VertexGeneric& vertex = vertices[ii];

			vertex.Position = DirectX::XMFLOAT3(pos.GetX(), pos.GetY(), pos.GetZ());
			vertex.Color = DirectX::XMFLOAT3(color.GetX(), color.GetY(), color.GetZ());
			vertex.Uv = DirectX::XMFLOAT2(uv.GetX(), uv.GetY());
		}

		int indexCount = static_cast<int>(m_indices.GetSize());
		m_mesh.LoadVertexAndIndexBuffer(vertices.data(), vertexCount, m_indices.GetData(), indexCount);
	}
}
