/********************************************************************/
/* � 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/LevelEditor/LevelEditorModule.h"

#include "Core/Math/Vec4f.h"

#include "Editors/LevelEditor/Component.h"
#include "Editors/LevelEditor/LevelMgr.h"
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

	bool LevelEditorModule::RenameLevel(Systems::NewAssetId id, const std::string& newName)
	{
		Systems::AssetMetadata* pMetadata = Systems::AssetMgr::Get().GetMetadata(id);
		if (!pMetadata)
			return false;

		pMetadata->SetVirtualName(newName);

		bool res = Systems::AssetMgr::Get().SaveMetadataTable();
		if (!res)
			return false;

		m_onRenameLevel(id, newName);

		return true;;
	}

	void LevelEditorModule::AddGameObject(const Core::Guid& parentGuid, Core::Guid& newGoGuid)
	{
		if (!m_pLevel)
			return;

		Systems::GameObject* pGo = Systems::CreateNewGameObject<Systems::GameObject>("newentity");
		
		Core::Mat44f localTx;
		localTx.SetIdentity();
		pGo->GetTransform().SetLocalTx(localTx);
		
		m_pLevel->AddGameObject(pGo, parentGuid);

		newGoGuid = pGo->GetGuid();

		const Systems::GameObject* pGoParent = nullptr;
		if (parentGuid.IsValid())
			pGoParent = m_pLevel->FindGameObject(parentGuid);

		m_onAddGameObject(pGo, pGoParent);
	}

	void LevelEditorModule::DeleteGameObject(const Core::Guid& guid)
	{
		if (!m_pLevel)
			return;

		m_pSelectionMgr->Remove(guid);
		
		m_onBeforeDeleteGameObject(guid);

		m_pLevel->DeleteGameObject(guid);

		m_onAfterDeleteGameObject(guid);
	}

	void LevelEditorModule::RenameGameObject(const Core::Guid& guid, const std::string& name)
	{
		if (!m_pLevel)
			return;

		Systems::GameObject* pGo = m_pLevel->FindGameObject(guid);
		if (!pGo)
			return;

		pGo->SetName(name);

		m_onRenameGameObject(guid, name);
	}

	void LevelEditorModule::DuplicateEntity(const Core::Guid& originalNode, Core::Guid& newNode)
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

	void LevelEditorModule::ReparentGameObject(const Core::Guid& parent, const Core::Guid& child)
	{
		//first check all the inputs are valid
		if (!m_pLevel)
			return;

		Systems::GameObject* pGoChild = m_pLevel->FindGameObject(child);
		if (!pGoChild)
			return;

		Systems::TransformComponent& childTx = pGoChild->GetTransform();
		Core::Guid oldParentGuid = childTx.GetParentGuid();
		Systems::GameObject* pGoOldParent = m_pLevel->FindGameObject(oldParentGuid);

		Systems::GameObject* pGoNewParent = nullptr;
		if (parent.IsValid())
		{
			pGoNewParent = m_pLevel->FindGameObject(parent);
			if (!pGoNewParent)
				return;
		}

		//if the new parent and old parent are the same then there is nothing to do
		if (pGoNewParent == pGoOldParent)
			return;

		//add the child to its new parent if any
		if (pGoNewParent)
			pGoNewParent->GetTransform().AddChild(child);

		//remove the child from its old parent
		if (pGoOldParent)
		{
			Systems::TransformComponent& oldParentTx = pGoOldParent->GetTransform();
			oldParentTx.RemoveChild(pGoChild->GetGuid());
		}

		//set the new parent to the child
		pGoChild->GetTransform().SetParentGuid(parent);

		m_onReparentGameObject(pGoChild, pGoOldParent, pGoNewParent);
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

	bool LevelEditorModule::AddToSelection(const Core::Guid& nodeGuid)
	{
		return m_pSelectionMgr->Add(nodeGuid);
	}

	bool LevelEditorModule::RemoveFromSelection(const Core::Guid& nodeGuid)
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

	const Systems::LevelAsset* LevelEditorModule::GetCurrentLoadedLevel() const
	{
		return m_pLevel;
	}

	Systems::LevelAsset* LevelEditorModule::GetCurrentLoadedLevel()
	{
		return m_pLevel;
	}
}
