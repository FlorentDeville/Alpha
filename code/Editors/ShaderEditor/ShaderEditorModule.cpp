/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/ShaderEditor/ShaderEditorModule.h"

#include "Editors/ShaderEditor/Compiler/MaterialParameters.h"
#include "Editors/ShaderEditor/Compiler/ShaderCompiler.h"

#include "Systems/Assets/AssetMgr.h"
#include "Systems/Assets/AssetObjects/AssetUtil.h"
#include "Systems/Container/Container.h"
#include "Systems/Container/ContainerMgr.h"

namespace Editors
{
	bool CompileSingleShader(const std::string& filename, Core::Array<char>& bytecode)
	{
		const std::string tempCsoFile = "c:\\tmp\\shaderblob.cso";
		ShaderCompiler compiler;
		bool res = compiler.CompileShader(filename, tempCsoFile);
		if (!res)
			return false;

		//load the cso file
		FILE* pFile = nullptr;
		fopen_s(&pFile, tempCsoFile.c_str(), "rb");
		if (!pFile)
			return false;

		fseek(pFile, 0, SEEK_END);
		uint64_t size = ftell(pFile);
		fseek(pFile, 0, SEEK_SET);
		if (size == -1L)
			return false;

		bytecode.Resize(static_cast<uint32_t>(size));

		size_t bytesRead = fread(bytecode.GetData(), sizeof(char), size, pFile);
		if (bytesRead != size)
		{
			int endOfFile = feof(pFile);
			return false;
		}

		fclose(pFile);

		return true;
	}

	void ShaderEditorModule::Init()
	{
		const Systems::AssetMgr& assetMgr = Systems::AssetMgr::Get();

		assetMgr.ForEachMetadata([this](const Systems::AssetMetadata& metadata)
			{
				if (metadata.GetAssetType() != MAKESID("Material"))
					return;

				m_allShaders.push_back(metadata.GetAssetId());
			});
	}

	Systems::MaterialAsset* ShaderEditorModule::NewShader(const std::string& virtualName)
	{
		Systems::MaterialAsset* pNewMaterial = Systems::CreateNewAsset<Systems::MaterialAsset>();
		assert(pNewMaterial);

		Systems::ContainerMgr& containerMgr = Systems::ContainerMgr::Get();
		Systems::Container* pContainer = containerMgr.CreateContainer(virtualName.c_str());
		pContainer->AddAsset(pNewMaterial);
		bool res = containerMgr.SaveContainer(pContainer->GetId());
		if (!res)
			return nullptr;

		Systems::AssetMetadata materialMetadata(pNewMaterial->GetId(), virtualName, MAKESID("Material"));
		Systems::AssetMgr& assetMgr = Systems::AssetMgr::Get();
		res = assetMgr.RegisterAssetMetadata(materialMetadata);
		if (!res)
			return nullptr;

		res = assetMgr.SaveMetadataTable();
		if (!res)
			return nullptr;

		m_allShaders.push_back(pNewMaterial->GetId());

		m_onShaderCreated(&materialMetadata);

		return pNewMaterial;
	}

	bool ShaderEditorModule::SaveShader(Systems::NewAssetId id)
	{
		Systems::ContainerMgr& containerMgr = Systems::ContainerMgr::Get();
		bool res = containerMgr.SaveContainer(id.GetContainerId());
		return res;
	}

	bool ShaderEditorModule::DeleteShader(Systems::NewAssetId id)
	{
		bool res = Systems::AssetUtil::DeleteAsset(id);
		if (!res)
			return false;

		std::vector<Systems::NewAssetId>::const_iterator it = std::find(m_allShaders.cbegin(), m_allShaders.cend(), id);
		if (it == m_allShaders.cend())
			return true;

		m_allShaders.erase(it);
		return true;
	}

	const std::vector<Systems::NewAssetId>& ShaderEditorModule::GetAllShaders() const
	{
		return m_allShaders;
	}

	bool ShaderEditorModule::CompileShader(Systems::NewAssetId id)
	{
		Systems::ContainerMgr& containerMgr = Systems::ContainerMgr::Get();
		Systems::Container* pContainer = containerMgr.GetContainer(id.GetContainerId());
		if (!pContainer)
			return false;

		Systems::MaterialAsset* pMaterial = static_cast<Systems::MaterialAsset*>(pContainer->GetAsset(id.GetObjectId()));
		if (!pMaterial)
			return false;

		CompileSingleShader(pMaterial->GetSourceFileVs(), pMaterial->GetVsBlob());
		CompileSingleShader(pMaterial->GetSourceFilePs(), pMaterial->GetPsBlob());

		ShaderCompiler compiler;
		bool res = compiler.GenerateRootSignature(pMaterial->GetPsBlob(), pMaterial->GetVsBlob(), pMaterial->GetRsBlob());
		if (!res)
			return false;

		MaterialParameters parameters;
		res = compiler.GenerateMaterialParameters(pMaterial->GetPsBlob(), pMaterial->GetVsBlob(), parameters);
		if (!res)
			return false;

		pMaterial->SetHasPerFrameParameters(parameters.m_hasPerFrameParameters);
		pMaterial->SetHasPerObjectParameters(parameters.m_hasPerObjectParameters);

		Core::Array<Systems::MaterialParameterDescription>& matParamDesc = pMaterial->GetMaterialParameterDescription();
		matParamDesc.Resize(0);
		matParamDesc.Reserve(static_cast<uint32_t>(parameters.m_perMaterialParameters.size()));
		for (const MaterialParameter& param : parameters.m_perMaterialParameters)
		{
			Systems::MaterialParameterDescription newParamDesc;
			newParamDesc.m_name = param.m_name;
			newParamDesc.m_offset = param.m_offset;
			newParamDesc.m_size = param.m_size;
			newParamDesc.m_type = param.m_type;
			newParamDesc.m_strType = param.m_strType;
			matParamDesc.PushBack(newParamDesc);
		}

		pMaterial->UpdateRenderingObjects();

		return res;
	}
}
