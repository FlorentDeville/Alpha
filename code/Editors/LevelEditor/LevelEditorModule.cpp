/********************************************************************/
/* © 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/LevelEditor/LevelEditorModule.h"

#include "Core/Math/Vec4f.h"

#include "Editors/LevelEditor/Component.h"
#include "Editors/LevelEditor/LevelMgr.h"
#include "Editors/LevelEditor/LevelSerializer.h"
#include "Editors/LevelEditor/SceneTree/Entity.h"
#include "Editors/LevelEditor/SceneTree/SceneTree.h"
#include "Editors/LevelEditor/SelectionMgr.h"

#include "Rendering/Material/MaterialMgr.h"
#include "Rendering/Mesh/MeshMgr.h"

#include "Systems/Assets/Asset.h"
#include "Systems/Assets/AssetMgr.h"
#include "Systems/Assets/AssetObjects/AssetUtil.h"
#include "Systems/Loader.h"

//#pragma optimize("", off)

namespace Editors
{
	Component* CreateComponentTransform()
	{
		Component* pTransform = new Component("Transform");

		Property* pTransformProperty = new Property("Local", Core::Mat44f());
		pTransform->AddProperty(pTransformProperty);

		return pTransform;
	}

	Component* CreateComponentRendering()
	{
		Component* pRendering = new Component("Rendering");

		Property* pMeshProperty = new Property("Mesh", kAssetMesh, Systems::AssetId::INVALID);
		Property* pMaterialProperty = new Property("Material", kAssetMaterial, Systems::AssetId::INVALID);

		pRendering->AddProperty(pMeshProperty);
		pRendering->AddProperty(pMaterialProperty);

		return pRendering;
	}

	LevelEditorModule::LevelEditorModule()
		: m_fovRad(DirectX::XMConvertToRadians(45.f))
		, m_pLevelMgr(nullptr)
		, m_pSelectionMgr(nullptr)
		, m_loadedLevelAssetId()
		, m_pLevel(nullptr)
	{ }

	LevelEditorModule::~LevelEditorModule()
	{}

	void LevelEditorModule::Init()
	{
		InitializePropertyDescriptor();

		m_pLevelMgr = new LevelMgr();
		m_pSelectionMgr = new SelectionMgr();
	}

	void LevelEditorModule::Shutdown()
	{
		delete m_pSelectionMgr;
		m_pSelectionMgr = nullptr;

		delete m_pLevelMgr;
		m_pLevelMgr = nullptr;
	}

	const LevelMgr* LevelEditorModule::GetConstLevelMgr() const
	{
		return m_pLevelMgr;
	}

	LevelMgr* LevelEditorModule::GetLevelMgr()
	{
		return m_pLevelMgr;
	}

	const SelectionMgr* LevelEditorModule::GetConstSelectionMgr() const
	{
		return m_pSelectionMgr;
	}

	SelectionMgr* LevelEditorModule::GetSelectionMgr()
	{
		return m_pSelectionMgr;
	}

	Systems::LevelAsset* LevelEditorModule::CreateNewLevel(const std::string& levelName)
	{
		Systems::LevelAsset* pLevel = Systems::AssetUtil::CreateAsset<Systems::LevelAsset>(levelName);
		if (!pLevel)
			return nullptr;

		const Systems::AssetMetadata* pMetadata = Systems::AssetMgr::Get().GetMetadata(pLevel->GetId());

		m_onNewLevel(*pMetadata);

		return pLevel;
	}

	void LevelEditorModule::DeleteLevel(Systems::NewAssetId id)
	{
		const Systems::AssetMetadata* pMetadata = Systems::AssetMgr::Get().GetMetadata(id);
		if (!pMetadata)
			return;

		//make a copy of the metadata to use it in the after delete event
		Systems::AssetMetadata metadataCopy = *pMetadata;

		m_onBeforeDeleteLevel(metadataCopy);

		Systems::AssetUtil::DeleteAsset(id);

		m_onAfterDeleteLevel(metadataCopy);
	}

	bool LevelEditorModule::SaveLevel()
	{
		if (!m_loadedLevelAssetId.IsValid())
			return false;

		bool res = Systems::ContainerMgr::Get().SaveContainer(m_loadedLevelAssetId.GetContainerId());
		if (!res)
			return res;

		m_onSaveLevel();
		return true;
	}

	bool LevelEditorModule::OpenLevel(Systems::NewAssetId id)
	{
		Systems::LevelAsset* pLevel = Systems::AssetUtil::LoadAsset<Systems::LevelAsset>(id);
		if (!pLevel)
			return false;

		m_loadedLevelAssetId = id;
		m_pLevel = pLevel;

		m_onOpenLevel();

		return true;
	}

	void LevelEditorModule::AddNewEntity(Os::Guid& nodeGuid)
	{
		SceneTree* pSceneTree = m_pLevelMgr->GetSceneTree();

		Os::Guid parentGuid;
		if (Node* pParent = pSceneTree->GetRoot())
			parentGuid = pParent->GetConstGuid();
		
		std::string entityName = "newentity";

		Entity* pNewEntity = new Entity(entityName);
		Component* pPlanTransform = CreateComponentTransform();
		pPlanTransform->SetPropertyValue("Local", Core::Mat44f(
			Core::Vec4f(1, 0, 0, 0),
			Core::Vec4f(0, 1, 0, 0),
			Core::Vec4f(0, 0, 1, 0),
			Core::Vec4f(0, 0, 0, 1)));

		Component* pPlanRendering = CreateComponentRendering();
		pNewEntity->AddComponent(pPlanTransform);
		pNewEntity->AddComponent(pPlanRendering);

		pSceneTree->AddNode(pNewEntity, parentGuid);

		nodeGuid = pNewEntity->GetGuid();

		m_onAddEntity(pNewEntity->GetConstGuid());
	}

	void LevelEditorModule::DeleteEntity(const Os::Guid& nodeGuid)
	{
		m_pSelectionMgr->Remove(nodeGuid);
		bool res = m_pLevelMgr->GetSceneTree()->DeleteNode(nodeGuid);

		if (res)
			m_onDeleteEntity(nodeGuid);
	}

	void LevelEditorModule::RenameEntity(const Os::Guid& nodeGuid, const std::string& name)
	{
		Node* pNode = m_pLevelMgr->GetSceneTree()->GetNode(nodeGuid);
		if (!pNode)
			return;

		Entity* pEntity = pNode->ToEntity();
		if (!pEntity)
			return;

		pEntity->SetName(name);
		m_onRenameEntity(nodeGuid);
	}

	void LevelEditorModule::DuplicateEntity(const Os::Guid& originalNode, Os::Guid& newNode)
	{
		if (!originalNode.IsValid())
			return;

		SceneTree* pSceneTree = m_pLevelMgr->GetSceneTree();

		const Node* pOriginalNode = pSceneTree->GetConstNode(originalNode);
		if (!pOriginalNode)
			return;

		const Entity* pOriginalEntity = pOriginalNode->ToConstEntity();
		if (!pOriginalEntity)
			return;

		Entity* pNewEntity = new Entity(pOriginalEntity->GetName() + "_copy");
		newNode = pNewEntity->GetConstGuid();

		for (int ii = 0; ii < pOriginalEntity->GetComponentCount(); ++ii)
		{
			const Component* pOriginalComponent = pOriginalEntity->GetComponent(ii);
			Component* pCopyComponent = new Component(*pOriginalComponent);
			pNewEntity->AddComponent(pCopyComponent);
		}

		pSceneTree->AddNode(pNewEntity, pOriginalNode->GetConstParent()->GetConstGuid());

		if (m_onDuplicateEntity)
			m_onDuplicateEntity(originalNode, newNode);
	}

	void LevelEditorModule::SetCameraWs(const Core::Mat44f& ws)
	{
		m_cameraWs = ws;
	}

	const Core::Mat44f& LevelEditorModule::GetCameraWs() const
	{
		return m_cameraWs;
	}

	float LevelEditorModule::GetFovRad() const
	{
		return m_fovRad;
	}

	Rendering::MeshId LevelEditorModule::LoadMesh(Systems::AssetId id)
	{
		Systems::AssetMgr& assetMgr = Systems::AssetMgr::Get();
		Systems::Loader& loader = Systems::Loader::Get();
		Rendering::MeshMgr& meshMgr = Rendering::MeshMgr::Get();
		
		const Systems::Asset* pAsset = assetMgr.GetAsset(id);
		if (!pAsset)
			return Rendering::MeshId::INVALID;

		Rendering::Mesh* pMesh = nullptr;
		Rendering::MeshId meshId;
		
		meshMgr.CreateMesh(&pMesh, meshId);
		bool res = loader.LoadMesh(pAsset->GetPath(), *pMesh);
		if (!res)
			return Rendering::MeshId::INVALID;

		m_assetIdToMeshId[pAsset->GetId()] = meshId;

		return meshId;
	}

	Rendering::MaterialId LevelEditorModule::LoadMaterial(Systems::AssetId id)
	{
		Systems::AssetMgr& assetMgr = Systems::AssetMgr::Get();
		Systems::Loader& loader = Systems::Loader::Get();
		Rendering::MaterialMgr& materialMgr = Rendering::MaterialMgr::Get();

		const Systems::Asset* pAsset = assetMgr.GetAsset(id);
		if (!pAsset)
			return Rendering::MaterialId::INVALID;

		Rendering::Material* pMaterial = nullptr;
		Rendering::MaterialId materialId;

		materialMgr.CreateMaterial(&pMaterial, materialId);
		bool res = loader.LoadMaterial(pAsset->GetPath(), *pMaterial);
		if (!res)
			return Rendering::MaterialId::INVALID;

		m_assetIdToMaterialId[pAsset->GetId()] = materialId;

		return materialId;
	}

	bool LevelEditorModule::AddToSelection(const Os::Guid& nodeGuid)
	{
		return m_pSelectionMgr->Add(nodeGuid);
	}

	bool LevelEditorModule::RemoveFromSelection(const Os::Guid& nodeGuid)
	{
		return m_pSelectionMgr->Remove(nodeGuid);
	}

	void LevelEditorModule::ClearSelection()
	{
		m_pSelectionMgr->Clear();
	}

	Systems::NewAssetId LevelEditorModule::GetCurrentLoadedLevelAssetId() const
	{
		return m_loadedLevelAssetId;
	}

	std::string LevelEditorModule::GetCurrentLoadedLevelName() const
	{
		if (!m_loadedLevelAssetId.IsValid())
			return "";

		const Systems::AssetMetadata* pMetadata = Systems::AssetMgr::Get().GetMetadata(m_loadedLevelAssetId);
		if (!pMetadata)
			return "";

		return pMetadata->GetVirtualName();
	}
}
