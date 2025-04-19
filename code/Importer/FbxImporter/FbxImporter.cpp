/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Importer/FbxImporter/FbxImporter.h"

#include "ofbx.h"

namespace FbxImporter
{
	bool FbxImporter::Import(const std::string& fbxFilename, Systems::MeshAsset& mesh)
	{
		FILE* pFileHandle = nullptr;
		fopen_s(&pFileHandle, fbxFilename.c_str(), "rb");
		if (!pFileHandle)
			return false;

		fseek(pFileHandle, 0, SEEK_END);
		size_t fileSize = ftell(pFileHandle);
		fseek(pFileHandle, 0, SEEK_SET);

		ofbx::u8* pFileContent = new ofbx::u8[fileSize];
		fread(pFileContent, sizeof(ofbx::u8), fileSize, pFileHandle);

		fclose(pFileHandle);

		ofbx::IScene* pScene = ofbx::load(pFileContent, fileSize, static_cast<ofbx::u16>(ofbx::LoadFlags::NONE));
		if (!pScene)
		{
			delete[] pFileContent;
			return false;
		}

		const ofbx::Object* pRoot = pScene->getRoot();
		bool res = Visit(pRoot);

		delete[] pFileContent;
		if (!res || m_indices.GetSize() == 0)
			return false;
			

		mesh.Init(fbxFilename, m_position, m_uv, m_color, m_indices);

		return res;
	}

	bool FbxImporter::Visit(const ofbx::Object* pNode)
	{
		if (!pNode)
			return false;

		int ii = 0;
		while (ofbx::Object* pChild = pNode->resolveObjectLink(ii))
		{
			if (pChild->getType() == ofbx::Object::Type::MESH)
			{
				const ofbx::Mesh* pMesh = static_cast<ofbx::Mesh*>(pChild);
				bool res = ImportMesh(pMesh);
				if (!res)
					return res;
			}
			else
			{
				bool res = Visit(pChild);
				if (!res)
					return res;
			}

			++ii;
		}

		return true;
	}

	bool FbxImporter::ImportMesh(const ofbx::Mesh* pMesh)
	{
		const ofbx::GeometryData& data = pMesh->getGeometryData();

		ofbx::Vec3Attributes positions = data.getPositions();
		ofbx::Vec2Attributes uvs = data.getUVs(0);
		ofbx::Vec4Attributes colors = data.getColors();

		int partitionCount = data.getPartitionCount();

		for (int partitionIndex = 0; partitionIndex < partitionCount; ++partitionIndex)
		{
			ofbx::GeometryPartition partition = data.getPartition(partitionIndex);
			
			for (int polygonIndex = 0; polygonIndex < partition.polygon_count; ++polygonIndex)
			{
				const ofbx::GeometryPartition::Polygon polygon = partition.polygons[polygonIndex];
				int vertexStart = polygon.from_vertex;
				int vertexEnd = polygon.from_vertex + polygon.vertex_count;

				//dumb triangulation using the start pattern
				int triangleCount = vertexEnd - vertexStart - 2;
				for (int triangleIndex = 0; triangleIndex < triangleCount; ++triangleIndex)
				{
					//first vertex of the triangle is always the same
					uint16_t internalVertexIndex = static_cast<uint16_t>(m_position.GetSize());

					{
						ofbx::Vec3 pos = positions.values[positions.indices[vertexStart]];
						ofbx::Vec2 uv = uvs.values[uvs.indices[vertexStart]];
						//ofbx::Vec4 color = colors.values[colors.indices[vertexStart]];

						m_position.PushBack(Core::Vec4f(pos.x, pos.y, pos.z, 1));
						//m_color.push_back(Core::Vec4f(color.x, color.y, color.z, color.w));
						m_color.PushBack(Core::Vec4f(0, 0, 0, 0));
						m_uv.PushBack(Core::Vec4f(uv.x, uv.y, 0, 0));
					}

					{
						int secondVertexIndex = vertexStart + triangleIndex + 1;
						ofbx::Vec3 pos = positions.values[positions.indices[secondVertexIndex]];
						ofbx::Vec2 uv = uvs.values[uvs.indices[secondVertexIndex]];
						//ofbx::Vec4 color = colors.values[colors.indices[secondVertexIndex]];

						size_t internalVertexIndex = m_position.GetSize();
						m_position.PushBack(Core::Vec4f(pos.x, pos.y, pos.z, 1));
						//m_color.push_back(Core::Vec4f(color.x, color.y, color.z, color.w));
						m_color.PushBack(Core::Vec4f(0, 0, 0, 0));
						m_uv.PushBack(Core::Vec4f(uv.x, uv.y, 0, 0));
					}

					{
						int lastVertexIndex = vertexStart + triangleIndex + 2;
						ofbx::Vec3 pos = positions.values[positions.indices[lastVertexIndex]];
						ofbx::Vec2 uv = uvs.values[uvs.indices[lastVertexIndex]];
						//ofbx::Vec4 color = colors.values[colors.indices[lastVertexIndex]];

						size_t internalVertexIndex = m_position.GetSize();
						m_position.PushBack(Core::Vec4f(pos.x, pos.y, pos.z, 1));
						//m_color.push_back(Core::Vec4f(color.x, color.y, color.z, color.w));
						m_color.PushBack(Core::Vec4f(0, 0, 0, 0));
						m_uv.PushBack(Core::Vec4f(uv.x, uv.y, 0, 0));
					}

					m_indices.PushBack(internalVertexIndex);
					m_indices.PushBack(internalVertexIndex + 1);
					m_indices.PushBack(internalVertexIndex + 2);
				}
			}
			
		}
		
		return true;
	}
}
