/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/MaterialEditor/MaterialEditorModule.h"

#include "Core/Log/LogModule.h"

#include "Editors/MaterialEditor/Compiler/MaterialParameters.h"
#include "Editors/MaterialEditor/Compiler/RootSignatureDescription.h"
#include "Editors/MaterialEditor/Compiler/ShaderCompiler.h"
#include "Editors/MaterialEditor/ShaderAutoGen.h"

#include "Systems/Assets/AssetMgr.h"
#include "Systems/Assets/AssetObjects/AssetUtil.h"
#include "Systems/Assets/AssetObjects/Material/TextureBindingInfo.h"
#include "Systems/Container/Container.h"
#include "Systems/Container/ContainerMgr.h"

namespace Editors
{
	static bool CompileSingleShader(const std::string& filename, const std::string& includePath, Core::Array<char>& bytecode, bool debug)
	{
		std::string tempCsoFile = "c:\\tmp\\shaderblob.cso";
		if (debug)
		{
			size_t pos = filename.find_last_of('\\');
			tempCsoFile = "c:\\tmp\\" + filename.substr(pos) + ".cso";
		}

		ShaderCompiler compiler;
		bool res = compiler.CompileShader(filename, includePath, tempCsoFile, debug);
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

	void MaterialEditorModule::Init(const std::string& dataFolder)
	{
		//Generate all the shader header files
		m_autoGenShaderFolder = dataFolder;
		if (m_autoGenShaderFolder.back() != '\\' || m_autoGenShaderFolder.back() != '/')
			m_autoGenShaderFolder += '\\';

		m_autoGenShaderFolder += "intermediate\\autogenshaders\\";
		ShaderAutoGen::WriteAllHeaderFiles(m_autoGenShaderFolder);

		Systems::AssetMgr& assetMgr = Systems::AssetMgr::Get();
		
		assetMgr.ForEachMetadata([this](const Systems::AssetMetadata& metadata)
			{
				if (metadata.IsA<Systems::MaterialAsset>())
				{
					m_allMaterials.push_back(metadata.GetAssetId());
				}
				else if (metadata.IsA<Systems::MaterialInstanceAsset>())
				{
					m_allMaterials.push_back(metadata.GetAssetId());
					m_instancesMaterialCache.AddMaterialInstance(metadata.GetAssetId());
				}
			});
	}

	Systems::MaterialAsset* MaterialEditorModule::NewMaterial(const std::string& virtualName)
	{
		Systems::MaterialAsset* pNewMaterial = Systems::AssetUtil::CreateAsset<Systems::MaterialAsset>(virtualName);
		if (!pNewMaterial)
			return nullptr;

		m_allMaterials.push_back(pNewMaterial->GetId());

		const Systems::AssetMetadata* pMetadata = Systems::AssetMgr::Get().GetMetadata(pNewMaterial->GetId());
		m_onMaterialCreated(pMetadata);

		return pNewMaterial;
	}

	Systems::MaterialInstanceAsset* MaterialEditorModule::NewMaterialInstance(const std::string& virtualName, Systems::NewAssetId baseMaterialId)
	{
		Systems::MaterialInstanceAsset* pNewMaterialInstance = Systems::AssetUtil::CreateAsset<Systems::MaterialInstanceAsset>(virtualName);
		if (!pNewMaterialInstance)
			return nullptr;

		Systems::MaterialAsset* pBaseMaterial = Systems::AssetUtil::GetAsset<Systems::MaterialAsset>(baseMaterialId);
		if (!pBaseMaterial)
			return nullptr;

		bool res = pNewMaterialInstance->InitialiseFromBaseMaterial(pBaseMaterial);
		if (!res)
			return nullptr;

		m_allMaterials.push_back(pNewMaterialInstance->GetId());
		m_instancesMaterialCache.AddMaterialInstance(pNewMaterialInstance->GetId());

		const Systems::AssetMetadata* pMetadata = Systems::AssetMgr::Get().GetMetadata(pNewMaterialInstance->GetId());
		m_onMaterialInstanceCreated(pMetadata);

		return pNewMaterialInstance;
	}

	void MaterialEditorModule::RenameMaterial(Systems::NewAssetId id, const std::string newName)
	{
		if (!id.IsValid())
			return;

		Systems::AssetMgr& assetMgr = Systems::AssetMgr::Get();
		Systems::AssetMetadata* pMetadata = assetMgr.GetMetadata(id);
		if (!pMetadata)
			return;

		assetMgr.RenameAsset(id, newName);
		assetMgr.SaveMetadataTable();

		//Do not call m_onMaterialRenamed here. It will be called when AssetManager::OnAssetRenamed will be called.
		m_onMaterialRenamed(*pMetadata, newName);
	}

	bool MaterialEditorModule::SaveMaterial(Systems::NewAssetId id)
	{
		Systems::AssetMetadata* pMetadata = Systems::AssetMgr::Get().GetMetadata(id);
		if (!pMetadata)
			return false;

		if (pMetadata->GetAssetType() == Systems::MaterialAsset::GetAssetTypeNameSid())
		{
			const Systems::MaterialAsset* pMaterial = Systems::AssetUtil::GetAsset<Systems::MaterialAsset>(id);
			if (!pMaterial)
				return false;

			if (pMaterial->IsDebug())
			{
				Core::LogModule::Get().LogError("Cannot save debug material.");
				return false;
			}
		}

		Systems::ContainerMgr& containerMgr = Systems::ContainerMgr::Get();
		bool res = containerMgr.SaveContainer(id.GetContainerId());
		return res;
	}

	bool MaterialEditorModule::DeleteMaterial(Systems::NewAssetId id)
	{
		//first clean the caches
		std::vector<Systems::NewAssetId>::const_iterator it = std::find(m_allMaterials.cbegin(), m_allMaterials.cend(), id);
		if (it == m_allMaterials.cend())
			return true;

		m_allMaterials.erase(it);

		Systems::AssetMetadata* pMetadata = Systems::AssetMgr::Get().GetMetadata(id);
		if (pMetadata)
		{
			if (pMetadata->IsA<Systems::MaterialInstanceAsset>())
			{
				m_instancesMaterialCache.RemoveMaterialInstance(id);
			}
			else if (pMetadata->IsA<Systems::MaterialAsset>())
			{
				const Core::Array<Systems::NewAssetId>& instancesArray = m_instancesMaterialCache.GetMaterialInstances(id);
				for (Systems::NewAssetId instanceId : instancesArray)
				{
					if(Systems::MaterialInstanceAsset* pInstance = Systems::AssetUtil::GetAsset<Systems::MaterialInstanceAsset>(instanceId))
						pInstance->InitialiseFromBaseMaterial(nullptr);
				}
			}
		}

		//now remove the asset for real
		bool res = Systems::AssetUtil::DeleteAsset(id);
		if (!res)
			return false;
		
		return true;
	}

	const std::vector<Systems::NewAssetId>& MaterialEditorModule::GetAllMaterials() const
	{
		return m_allMaterials;
	}

	bool MaterialEditorModule::CompileMaterial(Systems::NewAssetId id, bool debug)
	{
		Systems::ContainerMgr& containerMgr = Systems::ContainerMgr::Get();
		Systems::Container* pContainer = containerMgr.GetContainer(id.GetContainerId());
		if (!pContainer)
			return false;

		Systems::AssetObject* pObj = pContainer->GetAsset(id.GetObjectId());
		if (!pObj->IsA<Systems::MaterialAsset>())
			return false;

		Systems::MaterialAsset* pMaterial = static_cast<Systems::MaterialAsset*>(pContainer->GetAsset(id.GetObjectId()));
		if (!pMaterial)
			return false;

		bool shaderCompiled = CompileSingleShader(pMaterial->GetSourceFileVs(), m_autoGenShaderFolder, pMaterial->GetVsBlob(), debug);
		if (!shaderCompiled)
			return false;

		shaderCompiled = CompileSingleShader(pMaterial->GetSourceFilePs(), m_autoGenShaderFolder, pMaterial->GetPsBlob(), debug);
		if (!shaderCompiled)
			return false;

		pMaterial->SetIsDebug(debug);

		RootSignatureDescription rootSignatureDesc;
		rootSignatureDesc.m_pRootSignatureBlob = &pMaterial->GetRsBlob();
		ShaderCompiler compiler;
		bool res = compiler.GenerateRootSignature(pMaterial->GetPsBlob(), pMaterial->GetVsBlob(), rootSignatureDesc);
		if (!res)
			return false;

		Core::Array<Systems::MaterialCBufferBindingInfo>& bindingInfoArray = pMaterial->GetBindingInfoArray();
		bindingInfoArray.Clear();
		bindingInfoArray.Reserve(static_cast<uint32_t>(rootSignatureDesc.m_parameters.GetSize()));
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

		//now store the textures
		Core::Array<Systems::TextureBindingInfo>& existingTextureBindings = pMaterial->GetTexturesBindingInfo();
		Core::Array<Systems::TextureBindingInfo> newTextureBindings;
		newTextureBindings.Reserve(rootSignatureDesc.m_textures.GetSize());
		for (const Editors::RootSigTexture& texture : rootSignatureDesc.m_textures)
		{
			//skip the shadow map, it's handle automatically
			if (texture.m_name == "shadowMap")
				continue;

			Systems::TextureBindingInfo textureBinding;
			textureBinding.m_name = texture.m_name;
			textureBinding.m_sigRootIndex = texture.m_rootSigIndex;
			
			//reset the old value if any
			const Core::Array<Systems::TextureBindingInfo>::Iterator it = std::find_if(existingTextureBindings.cbegin(), existingTextureBindings.cend(), 
				[&textureBinding](Systems::TextureBindingInfo& oldParam) { return textureBinding.m_name == oldParam.m_name; });
			if (it != existingTextureBindings.cend())
			{
				textureBinding.m_texture = it->m_texture;
			}

			newTextureBindings.PushBack(textureBinding);
		}

		existingTextureBindings = std::move(newTextureBindings);

		pMaterial->UpdateRenderingObjects();
		
		if (res)
			Core::LogModule::Get().LogInfo("Material compiled successfully.");

		return res;
	}

	void MaterialEditorModule::BaseToInstanceCache::AddMaterialInstance(Systems::NewAssetId id)
	{
		const Systems::MaterialInstanceAsset* pInstance = Systems::AssetUtil::LoadAsset<Systems::MaterialInstanceAsset>(id);
		if (!pInstance)
			return;

		if (!pInstance->GetBaseMaterialId().IsValid())
			return;

		m_baseToInstances[pInstance->GetBaseMaterialId()].PushBack(id);
	}

	void MaterialEditorModule::BaseToInstanceCache::RemoveMaterialInstance(Systems::NewAssetId id)
	{
		const Systems::MaterialInstanceAsset* pInstance = Systems::AssetUtil::LoadAsset<Systems::MaterialInstanceAsset>(id);
		if (!pInstance)
			return;

		if (!pInstance->GetBaseMaterialId().IsValid())
			return;

		std::map<Systems::NewAssetId, Core::Array<Systems::NewAssetId>>::iterator it = m_baseToInstances.find(pInstance->GetBaseMaterialId());
		if (it == m_baseToInstances.end())
			return;

		Core::Array<Systems::NewAssetId>& instancesArray = it->second;
		instancesArray.Erase(id);
	}

	const Core::Array<Systems::NewAssetId>& MaterialEditorModule::BaseToInstanceCache::GetMaterialInstances(Systems::NewAssetId baseMaterialId) const
	{
		static Core::Array<Systems::NewAssetId> defaultRet;

		std::map<Systems::NewAssetId, Core::Array<Systems::NewAssetId>>::const_iterator it = m_baseToInstances.find(baseMaterialId);
		if (it == m_baseToInstances.cend())
			return defaultRet;

		return it->second;
	}
}
