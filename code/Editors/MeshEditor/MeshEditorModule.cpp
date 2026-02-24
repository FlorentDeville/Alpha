/********************************************************************/
/* © 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/MeshEditor/MeshEditorModule.h"

#include "Importer/FbxImporter/FbxImporter.h"

#include "Systems/Assets/AssetObjects/AssetUtil.h"
#include "Systems/Container/ContainerMgr.h"

namespace Editors
{
	MeshEditorModule::MeshEditorModule()
		: Core::Singleton<MeshEditorModule>()
	{ }

	MeshEditorModule::~MeshEditorModule()
	{ }

	bool MeshEditorModule::SaveMesh(const Systems::NewAssetId id)
	{
		if (!id.IsValid())
			return false;

		bool res = Systems::ContainerMgr::Get().SaveContainer(id.GetContainerId());
		return res;
	}

	bool MeshEditorModule::DeleteMesh(const Systems::NewAssetId id)
	{
		Systems::AssetMgr& assetMgr = Systems::AssetMgr::Get();
		Systems::AssetMetadata* pMetadata = assetMgr.GetMetadata(id);
		if (!pMetadata)
			return false;

		if (!pMetadata->IsA<Systems::MeshAsset>())
			return false;

		Systems::AssetMetadata copyMetadata(*pMetadata);
		m_onBeforeMeshDeleted(copyMetadata);

		bool res = Systems::AssetUtil::DeleteAsset(id);
		if (!res)
			return res;

		m_onAfterMeshDeleted(copyMetadata);

		return true;
	}

	bool MeshEditorModule::ImportMesh(const std::string& filename)
	{
		Systems::MeshAsset* pNewMesh = Systems::CreateNewAsset<Systems::MeshAsset>();
		FbxImporter::FbxImporter importer;
		bool res = importer.Import(filename, *pNewMesh);

		if (!res)
		{
			Systems::DeleteObject(pNewMesh);
			return res;
		}

		Systems::ContainerMgr& containerMgr = Systems::ContainerMgr::Get();
		Systems::AssetMgr& assetMgr = Systems::AssetMgr::Get();

		Systems::ContainerId cid = assetMgr.GenerateNewContainerId();
		Systems::Container* pContainer = containerMgr.CreateContainer(cid);
		pContainer->AddAsset(pNewMesh);

		size_t start = filename.find_last_of('\\');
		std::string assetName = filename.substr(start + 1);
		size_t end = assetName.find_last_of('.');
		assetName = assetName.substr(0, end);

		Systems::AssetMetadata metadata(pNewMesh->GetId(), assetName, Systems::MeshAsset::GetAssetTypeNameSid());
		assetMgr.RegisterAssetMetadata(metadata);
		assetMgr.SaveMetadataTable();

		containerMgr.SaveContainer(pNewMesh->GetId().GetContainerId());
		
		m_onMeshCreated(metadata);

		return res;
	}

	bool MeshEditorModule::RenameMesh(const Systems::NewAssetId id, const std::string& newName)
	{
		Systems::AssetMgr& assetMgr = Systems::AssetMgr::Get();
		Systems::AssetMetadata* pMetadata = assetMgr.GetMetadata(id);
		if (!pMetadata)
			return false;

		pMetadata->SetVirtualName(newName);
		assetMgr.SaveMetadataTable();

		m_onMeshRenamed(*pMetadata);
		return true;
	}
}