/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Systems/Loader.h"

#include <cstdio>

#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"

#include "Rendering/Mesh/Mesh.h"

namespace Systems
{
	Loader::Loader()
		: Core::Singleton<Loader>()
		, m_dataMeshPath()
	{}

	Loader::~Loader()
	{}

	bool Loader::Init(const std::string& dataMeshPath)
	{
		m_dataMeshPath = dataMeshPath;

		return true;
	}

	bool Loader::LoadMesh(const std::string& name, Rendering::Mesh& mesh)
	{
		std::string filename = m_dataMeshPath + "\\" + name + ".json";

		FILE* pFile = nullptr;
		fopen_s(&pFile, filename.c_str(), "rb");
		assert(pFile);

		const int BUFFER_SIZE = 655536;
		char readBuffer[BUFFER_SIZE];
		rapidjson::FileReadStream stream(pFile, readBuffer, BUFFER_SIZE);

		rapidjson::Document jsonDocument;
		jsonDocument.ParseStream(stream);

		const rapidjson::Value& indexBuffer = jsonDocument["index_buffer"];
		int indicesCount = indexBuffer.Size();
		uint16_t* pIndices = new uint16_t[indicesCount];
		for (int ii = 0; ii < indicesCount; ++ii)
		{
			pIndices[ii] = indexBuffer[ii].GetUint();
		}

		const char* vertexStructure = jsonDocument["vertex_structure"].GetString();
		if (strcmp(vertexStructure, "pos,col") == 0)
		{
			const int ELEMENT_COUNT = 6;
			const rapidjson::Value& vertices = jsonDocument["vertex_buffer"];
			int verticesCount = vertices.Size() / ELEMENT_COUNT;

			Rendering::VertexPosColor* pVertices = new Rendering::VertexPosColor[verticesCount];

			for (int ii = 0; ii < verticesCount; ++ii)
			{
				int start = ii * ELEMENT_COUNT;
				pVertices[ii].Position.x = vertices[start].GetFloat();
				pVertices[ii].Position.y = vertices[start + 1].GetFloat();
				pVertices[ii].Position.z = vertices[start + 2].GetFloat();
				pVertices[ii].Color.x = vertices[start + 3].GetFloat();
				pVertices[ii].Color.y = vertices[start + 4].GetFloat();
				pVertices[ii].Color.z = vertices[start + 5].GetFloat();
			}

			mesh.LoadVertexAndIndexBuffer(pVertices, verticesCount, pIndices, indicesCount);
		}
		else if (strcmp(vertexStructure, "pos,col,uv") == 0)
		{
			const int ELEMENT_COUNT = 8;
			const rapidjson::Value& vertices = jsonDocument["vertex_buffer"];
			int verticesCount = vertices.Size() / ELEMENT_COUNT;

			Rendering::VertexGeneric* pVertices = new Rendering::VertexGeneric[verticesCount];

			for (int ii = 0; ii < verticesCount; ++ii)
			{
				int start = ii * ELEMENT_COUNT;
				pVertices[ii].Position.x = vertices[start].GetFloat();
				pVertices[ii].Position.y = vertices[start + 1].GetFloat();
				pVertices[ii].Position.z = vertices[start + 2].GetFloat();
				pVertices[ii].Color.x = vertices[start + 3].GetFloat();
				pVertices[ii].Color.y = vertices[start + 4].GetFloat();
				pVertices[ii].Color.z = vertices[start + 5].GetFloat();
				pVertices[ii].Uv.x = vertices[start + 6].GetFloat();
				pVertices[ii].Uv.y = vertices[start + 7].GetFloat();
			}

			mesh.LoadVertexAndIndexBuffer(pVertices, verticesCount, pIndices, indicesCount);
		}
		else
		{
			//unknown type of vertex structure
			assert(false);
		}

		//delete[] pIndices;

		fclose(pFile);

		return true;
	}

	bool Loader::LoadMaterial(const std::string& name, Rendering::Material& material)
	{
		return true;
	}
}
