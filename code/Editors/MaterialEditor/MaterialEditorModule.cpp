/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/MaterialEditor/MaterialEditorModule.h"

#include "Core/Log/LogModule.h"

#include "Editors/MaterialEditor/Compiler/MaterialParameters.h"
#include "Editors/MaterialEditor/Compiler/RootSignatureDescription.h"
#include "Editors/MaterialEditor/Compiler/ShaderCompiler.h"

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

	void MaterialEditorModule::Init()
	{
		const Systems::AssetMgr& assetMgr = Systems::AssetMgr::Get();

		assetMgr.ForEachMetadata([this](const Systems::AssetMetadata& metadata)
			{
				if (metadata.GetAssetType() != MAKESID("Material"))
					return;

				m_allShaders.push_back(metadata.GetAssetId());
			});
	}

	Systems::MaterialAsset* MaterialEditorModule::NewMaterial(const std::string& virtualName)
	{
		Systems::MaterialAsset* pNewMaterial = Systems::CreateNewAsset<Systems::MaterialAsset>();
		assert(pNewMaterial);

		Systems::ContainerMgr& containerMgr = Systems::ContainerMgr::Get();
		Systems::Container* pContainer = containerMgr.CreateContainer(virtualName.c_str());
		pContainer->AddAsset(pNewMaterial);
		bool res = containerMgr.SaveContainer(pContainer->GetId());
		if (!res)
			return nullptr;

		Systems::AssetMetadata materialMetadata(pNewMaterial->GetId(), virtualName, Systems::MaterialAsset::GetAssetTypeNameSid());
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

	bool MaterialEditorModule::SaveMaterial(Systems::NewAssetId id)
	{
		Systems::ContainerMgr& containerMgr = Systems::ContainerMgr::Get();
		bool res = containerMgr.SaveContainer(id.GetContainerId());
		return res;
	}

	bool MaterialEditorModule::DeleteMaterial(Systems::NewAssetId id)
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

	const std::vector<Systems::NewAssetId>& MaterialEditorModule::GetAllShaders() const
	{
		return m_allShaders;
	}

	bool MaterialEditorModule::CompileMaterial(Systems::NewAssetId id)
	{
		Systems::ContainerMgr& containerMgr = Systems::ContainerMgr::Get();
		Systems::Container* pContainer = containerMgr.GetContainer(id.GetContainerId());
		if (!pContainer)
			return false;

		Systems::MaterialAsset* pMaterial = static_cast<Systems::MaterialAsset*>(pContainer->GetAsset(id.GetObjectId()));
		if (!pMaterial)
			return false;

		bool shaderCompiled = CompileSingleShader(pMaterial->GetSourceFileVs(), pMaterial->GetVsBlob());
		if (!shaderCompiled)
			return false;

		shaderCompiled = CompileSingleShader(pMaterial->GetSourceFilePs(), pMaterial->GetPsBlob());
		if (!shaderCompiled)
			return false;

		RootSignatureDescription rootSignatureDesc;
		rootSignatureDesc.m_pRootSignatureBlob = &pMaterial->GetRsBlob();
		ShaderCompiler compiler;
		bool res = compiler.GenerateRootSignature(pMaterial->GetPsBlob(), pMaterial->GetVsBlob(), rootSignatureDesc);
		if (!res)
			return false;

		Core::Array<Systems::MaterialCBufferBindingInfo>& bindingInfoArray = pMaterial->GetBindingInfoArray();
		bindingInfoArray.Reserve(static_cast<uint32_t>(rootSignatureDesc.m_parameters.size()));
		for (const RootSigParameterIndex& paramIndex : rootSignatureDesc.m_parameters)
		{
			Systems::MaterialCBufferBindingInfo bindingInfo;

			if (paramIndex.m_cbufferName == "PerMaterial")
			{
				bindingInfo.m_type = Systems::CBufferType::PerMaterial;
			}
			else if (paramIndex.m_cbufferName == "PerObject")
			{
				bindingInfo.m_type = Systems::CBufferType::PerObject;
			}
			else if (paramIndex.m_cbufferName == "PerFrame")
			{
				bindingInfo.m_type = Systems::CBufferType::PerFrame;
			}
			else if (paramIndex.m_cbufferName == "CBufferLights")
			{
				bindingInfo.m_type = Systems::CBufferType::Lights;
			}
			else
			{
				continue;
			}

			bindingInfo.m_sigRootParamIndex = paramIndex.m_rootSigParamIndex;
			bindingInfoArray.PushBack(bindingInfo);
		}

		MaterialParameters parameters;
		res = compiler.GenerateMaterialParameters(pMaterial->GetPsBlob(), pMaterial->GetVsBlob(), parameters);
		if (!res)
			return false;

		Core::Array<Systems::MaterialParameterDescription>& existingMatParamArray = pMaterial->GetMaterialParameterDescription();

		Core::Array<Systems::MaterialParameterDescription> newMatParamDescArray;
		newMatParamDescArray.Reserve(static_cast<uint32_t>(parameters.m_perMaterialParameters.size()));
		for (const MaterialParameter& param : parameters.m_perMaterialParameters)
		{
			Systems::MaterialParameterDescription newParamDesc;
			newParamDesc.m_name = param.m_name;
			newParamDesc.m_offset = param.m_offset;
			newParamDesc.m_size = param.m_size;
			newParamDesc.m_type = param.m_type;
			newParamDesc.m_strType = param.m_strType;

			//if a parameter with the same name and size already exists, keep its value
			bool copyValue = false;
			const Core::Array<Systems::MaterialParameterDescription>::Iterator it = std::find_if(existingMatParamArray.cbegin(), existingMatParamArray.cend(), [&param](Systems::MaterialParameterDescription& oldParam) { return param.m_name == oldParam.m_name; });
			if (it != existingMatParamArray.cend())
			{
				if (it->m_size == param.m_size)
				{
					copyValue = true;
				}
			}

			if (copyValue)
			{
				newParamDesc.m_value = std::move(it->m_value);
			}
			else
			{
				newParamDesc.m_value.Resize(param.m_size, 0);
			}
			newMatParamDescArray.PushBack(newParamDesc);
		}

		existingMatParamArray = std::move(newMatParamDescArray);

		pMaterial->UpdateRenderingObjects();
		
		if (res)
			Core::LogModule::Get().LogInfo("Material compiled successfully.");

		return res;
	}
}
