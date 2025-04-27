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

	const std::string& MeshAsset::GetSourceFile() const
	{
		return m_sourceFile;
	}

	const Rendering::Mesh* MeshAsset::GetRenderingMesh() const
	{
		return &m_mesh;
	}

	void MeshAsset::PostLoad()
	{
		uint32_t vertexCount = m_position.GetSize();
		std::vector<Rendering::VertexGeneric> vertices;
		vertices.resize(vertexCount);

		for (uint32_t ii = 0; ii < vertexCount; ++ii)
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

	const Core::Array<Core::Vec4f>& MeshAsset::GetPositions() const
	{
		return m_position;
	}

	const Core::Array<Core::Vec4f>& MeshAsset::GetUvs() const
	{
		return m_uv;
	}

	const Core::Array<Core::Vec4f>& MeshAsset::GetColors() const
	{
		return m_color;
	}

	const Core::Array<uint16_t>& MeshAsset::GetIndices() const
	{
		return m_indices;
	}

	///////////////////////////////////////////////////////////////////////////////////////

	MeshAsset_v2::MeshAsset_v2()
		: AssetObject()
		, m_sourceFile()
		, m_position()
		, m_uv()
		, m_color()
		, m_indices()
		, m_mesh()
	{ }

	MeshAsset_v2::~MeshAsset_v2()
	{ }

	void MeshAsset_v2::Init(const std::string& sourceFile, Core::Array<float>&& pos, Core::Array<float>&& uv, Core::Array<float>&& color, Core::Array<uint16_t>&& indices)
	{
		m_sourceFile = sourceFile;
		m_position = std::move(pos);
		m_uv = std::move(uv);
		m_color = std::move(color);
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

			Rendering::VertexGeneric& vertex = vertices[ii];

			vertex.Position = DirectX::XMFLOAT3(m_position[positionIndex], m_position[positionIndex + 1], m_position[positionIndex + 2]);
			vertex.Color = DirectX::XMFLOAT3(m_color[colorIndex], m_color[colorIndex + 1], m_color[colorIndex + 2]);
			vertex.Uv = DirectX::XMFLOAT2(m_uv[uvIndex], m_uv[uvIndex + 1]);
		}

		m_mesh.LoadVertexAndIndexBuffer(vertices.data(), vertexCount, m_indices.GetData(), indexCount);
	}

	void UpgradeMeshAssetV1ToV2(const MeshAsset* pMeshV1, MeshAsset_v2* pMeshV2)
	{
		const Core::Array<Core::Vec4f>& srcPos = pMeshV1->GetPositions();
		const Core::Array<Core::Vec4f>& srcUv = pMeshV1->GetUvs();
		const Core::Array<Core::Vec4f>& srcColor = pMeshV1->GetColors();
		const Core::Array<uint16_t>& srcIndices = pMeshV1->GetIndices();

		Core::Array<float> dstPos;
		dstPos.Reserve(srcIndices.GetSize() * 3);
		for (uint32_t ii = 0; ii < srcPos.GetSize(); ++ii)
		{
			const Core::Vec4f& src = srcPos[ii];
			dstPos.PushBack(src.GetX());
			dstPos.PushBack(src.GetY());
			dstPos.PushBack(src.GetZ());
		}

		Core::Array<float> dstUv;
		dstUv.Reserve(srcIndices.GetSize() * 2);
		for (uint32_t ii = 0; ii < srcUv.GetSize(); ++ii)
		{
			const Core::Vec4f& src = srcUv[ii];
			dstUv.PushBack(src.GetX());
			dstUv.PushBack(src.GetY());
		}

		Core::Array<float> dstColor;
		dstPos.Reserve(srcIndices.GetSize() * 4);
		for (uint32_t ii = 0; ii < srcColor.GetSize(); ++ii)
		{
			const Core::Vec4f& src = srcColor[ii];
			dstColor.PushBack(src.GetX());
			dstColor.PushBack(src.GetY());
			dstColor.PushBack(src.GetZ());
			dstColor.PushBack(src.GetW());
		}

		Core::Array<uint16_t> dstIndices;
		dstIndices.Reserve(srcIndices.GetSize());
		for (uint32_t ii = 0; ii < srcIndices.GetSize(); ++ii)
		{
			dstIndices.PushBack(srcIndices[ii]);
		}

		pMeshV2->Init(pMeshV1->GetSourceFile(), std::move(dstPos), std::move(dstUv), std::move(dstColor), std::move(dstIndices));
	}
}
