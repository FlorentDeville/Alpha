/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Rendering/Material/Material.h"

#include "Rendering/PipelineState/PipelineStateMgr.h"
#include "Rendering/RenderModule.h"
#include "Rendering/RootSignature/RootSignatureMgr.h"
#include "Rendering/ShaderMgr.h"

#include <assert.h>

#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"



namespace Rendering
{
	Material::Material()
		: m_pRootSignature(nullptr)
		, m_pPipelineState(nullptr)
		, m_textureId(-1)
	{}

	Material::~Material()
	{}

	bool Material::Init(RootSignatureId rid, PipelineStateId pid)
	{
		m_pRootSignature = RootSignatureMgr::Get().GetRootSignature(rid);
		m_pPipelineState = PipelineStateMgr::Get().GetPipelineState(pid);

		return true;
	}

	void Material::SetTexture(TextureId id)
	{
		m_textureId = id;
	}

	bool Material::Load(const std::string& filename)
	{
		FILE* pFile = nullptr;;
		fopen_s(&pFile, filename.c_str(), "rb");
		assert(pFile);

		const int BUFFER_SIZE = 655536;
		char readBuffer[BUFFER_SIZE];
		rapidjson::FileReadStream stream(pFile, readBuffer, BUFFER_SIZE);

		rapidjson::Document jsonDocument;
		jsonDocument.ParseStream(stream);

		const rapidjson::Value& shaderNameValue = jsonDocument["shader"];
		const char* shaderName = shaderNameValue.GetString();

		std::string root = "C:\\workspace\\Alpha\\data\\shaders\\";
		RootSignatureId rsId = Rendering::RootSignatureMgr::Get().CreateRootSignature(root + "\\" + shaderName + ".rs.cso");
		ShaderId vsId = g_pShaderMgr->CreateShader(root + "\\" + shaderName + ".vs.cso");
		ShaderId psId = g_pShaderMgr->CreateShader(root + "\\" + shaderName + ".ps.cso");

		Rendering::PipelineStateId pid;
		Rendering::PipelineState* pPipelineState = Rendering::PipelineStateMgr::Get().CreatePipelineState(pid);
		pPipelineState->Init_Generic(rsId, vsId, psId);

		Init(rsId, pid);

		rapidjson::Value::MemberIterator it = jsonDocument.FindMember("texture");
		if (it != jsonDocument.MemberEnd())
		{
			const char* textureName = it->value.GetString();
			std::string root = "C:\\workspace\\Alpha\\data\\textures\\";
			
			TextureId tid;
			Texture* pTexture = ::RenderModule::Get().GetTextureMgr().CreateResource(tid, root + textureName);
			pTexture->Init(root + textureName);

			SetTexture(tid);
		}

		return true;
	}
}
