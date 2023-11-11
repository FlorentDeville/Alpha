/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Systems/Loader.h"

#include <cstdio>

#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"

#include "Rendering/Material/Material.h"
#include "Rendering/Mesh/Mesh.h"
#include "Rendering/PipelineState/PipelineState.h"
#include "Rendering/PipelineState/PipelineStateId.h"
#include "Rendering/PipelineState/PipelineStateMgr.h"
#include "Rendering/RenderModule.h"
#include "Rendering/RootSignature/RootSignatureId.h"
#include "Rendering/RootSignature/RootSignatureMgr.h"
#include "Rendering/Shaders/ShaderMgr.h"
#include "Rendering/Texture/Texture.h"
#include "Rendering/Texture/TextureMgr.h"

#include "Systems/Assets/Asset.h"
#include "Systems/Assets/AssetId.h"
#include "Systems/Assets/AssetMgr.h"

//#pragma optimize("", off)

namespace Systems
{
	Loader::Loader()
		: Core::Singleton<Loader>()
		, m_dataMeshPath()
		, m_dataMaterialPath()
		, m_dataShaderPath()
		, m_dataTexturePath()
	{}

	Loader::~Loader()
	{}

	bool Loader::Init(const LoaderParameter& parameter)
	{
		m_dataMeshPath = parameter.m_dataMeshPath;
		m_dataMaterialPath = parameter.m_dataMaterialPath;
		m_dataShaderPath = parameter.m_dataShaderPath;
		m_dataTexturePath = parameter.m_dataTexturePath;

		return true;
	}

	bool Loader::LoadMesh(const std::string& absFilename, Rendering::Mesh& mesh)
	{
		std::string filename = absFilename;

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

	bool Loader::LoadMaterial(const std::string& absFilename, Rendering::Material& material)
	{
		Rendering::ShaderMgr& shaderMgr = Rendering::ShaderMgr::Get();
		const Systems::AssetMgr& assetMgr = Systems::AssetMgr::Get();

		std::string materialFilename = absFilename;
		FILE* pFile = nullptr;;
		fopen_s(&pFile, materialFilename.c_str(), "rb");
		assert(pFile);

		const int BUFFER_SIZE = 655536;
		char readBuffer[BUFFER_SIZE];
		rapidjson::FileReadStream stream(pFile, readBuffer, BUFFER_SIZE);

		rapidjson::Document jsonDocument;
		jsonDocument.ParseStream(stream);

		const rapidjson::Value& shaderRSValue = jsonDocument["shader_rs_AID"];
		Systems::AssetId rsAid(shaderRSValue.GetInt64());
		const Systems::Asset* assetRS = assetMgr.GetAsset(rsAid);

		const rapidjson::Value& shaderVSValue = jsonDocument["shader_vs_AID"];
		Systems::AssetId vsAid(shaderVSValue.GetInt64());
		const Systems::Asset* assetVS = assetMgr.GetAsset(vsAid);

		const rapidjson::Value& shaderPSValue = jsonDocument["shader_ps_AID"];
		Systems::AssetId psAid(shaderPSValue.GetInt64());
		const Systems::Asset* assetPS = assetMgr.GetAsset(psAid);

		Rendering::RootSignatureId rsId = Rendering::RootSignatureMgr::Get().CreateRootSignature(assetRS->GetPath());
		Rendering::ShaderId vsId = shaderMgr.CreateShader(assetVS->GetPath());
		Rendering::ShaderId psId = shaderMgr.CreateShader(assetPS->GetPath());

		Rendering::PipelineStateId pid;
		Rendering::PipelineState* pPipelineState = Rendering::PipelineStateMgr::Get().CreatePipelineState(pid);
		pPipelineState->Init_Generic(rsId, vsId, psId);

		material.Init(rsId, pid);

		rapidjson::Value::MemberIterator it = jsonDocument.FindMember("texture_AID");
		if (it != jsonDocument.MemberEnd())
		{
			//get the texture asset
			Systems::AssetId textureAid(it->value.GetInt64());
			const Systems::Asset* pTextureAsset = assetMgr.GetAsset(textureAid);
			assert(pTextureAsset);

			//load the texture resource
			Rendering::TextureId tid;
			Rendering::Texture* pTexture = nullptr;

			Rendering::TextureMgr::Get().CreateTexture(&pTexture, tid);
			pTexture->Init(pTextureAsset->GetPath());

			material.SetTexture(tid);
		}

		return true;
	}
}
