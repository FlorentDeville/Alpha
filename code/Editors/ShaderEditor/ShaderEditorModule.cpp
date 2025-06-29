/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/ShaderEditor/ShaderEditorModule.h"

#include "Editors/ShaderEditor/Compiler/ShaderCompiler.h"

#include "Systems/Assets/AssetMgr.h"
#include "Systems/Assets/AssetObjects/AssetUtil.h"
#include "Systems/Container/Container.h"
#include "Systems/Container/ContainerMgr.h"

namespace Editors
{
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

		const std::string tempCsoFile = "c:\\tmp\\shaderblob.cso";
		ShaderCompiler Compiler;
		bool res = Compiler.CompileShader(pMaterial->GetSourceFilePs(), tempCsoFile);

		//load the cso file
		FILE* pFile = nullptr;
		fopen_s(&pFile, tempCsoFile.c_str(), "r");
		if (!pFile)
			return false;

		fseek(pFile, 0, SEEK_END);
		uint64_t size = ftell(pFile);
		fseek(pFile, 0, SEEK_SET);
		if (size == -1L)
			return false;

		Core::Array<char>& blob = pMaterial->GetPsBlob();
		blob.Resize(static_cast<uint32_t>(size));

		size_t bytesRead = fread(blob.GetData(), sizeof(char), size, pFile);
		if (bytesRead != size)
			return false;

		return res;
	}
}
