/********************************************************************/
/* © 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/LevelEditor/LevelEditorModule.h"

#include "Core/Math/Vec4f.h"

#include "Editors/LevelEditor/Component.h"
#include "Editors/LevelEditor/LevelMgr.h"
#include "Editors/LevelEditor/SceneTree/Entity.h"
#include "Editors/LevelEditor/SceneTree/SceneTree.h"
#include "Editors/LevelEditor/SelectionMgr.h"
#include "Editors/LevelEditor/Widgets/LevelEditorTab.h"

#include "Rendering/Material/MaterialMgr.h"
#include "Rendering/Mesh/MeshMgr.h"

#include "Systems/Assets/Asset.h"
#include "Systems/Assets/AssetMgr.h"
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

	void CreateLevel(LevelMgr& level, std::map<Systems::AssetId, Rendering::MeshId>& assetIdToMeshId, std::map<Systems::AssetId, Rendering::MaterialId>& assetIdToMaterialId)
	{
		//load all the resources we will use
		Systems::AssetId meshPlane(2);
		Systems::AssetId meshCube(1);
		Systems::AssetId meshTorus(3);
		Systems::AssetId materialBlue(4);
		Systems::AssetId materialOrange(5);
		Systems::AssetId materialVertexColor(6);

		Systems::AssetMgr& assetMgr = Systems::AssetMgr::Get();
		Rendering::MeshMgr& meshMgr = Rendering::MeshMgr::Get();
		Rendering::MaterialMgr& materialMgr = Rendering::MaterialMgr::Get();
		Systems::Loader& loader = Systems::Loader::Get();

		const Systems::Asset* pAsset = assetMgr.GetAsset(meshPlane);
		Rendering::Mesh* pMesh = nullptr;
		Rendering::MeshId meshId;
		{
			meshMgr.CreateMesh(&pMesh, meshId);
			loader.LoadMesh(pAsset->GetPath(), *pMesh);
			assetIdToMeshId[pAsset->GetId()] = meshId;
		}
		{
			pAsset = assetMgr.GetAsset(meshCube);
			meshMgr.CreateMesh(&pMesh, meshId);
			loader.LoadMesh(pAsset->GetPath(), *pMesh);
			assetIdToMeshId[pAsset->GetId()] = meshId;
		}
		{
			pAsset = assetMgr.GetAsset(meshTorus);
			meshMgr.CreateMesh(&pMesh, meshId);
			loader.LoadMesh(pAsset->GetPath(), *pMesh);
			assetIdToMeshId[pAsset->GetId()] = meshId;
		}

		Rendering::Material* pMaterial = nullptr;
		Rendering::MaterialId materialId;
		{
			pAsset = assetMgr.GetAsset(materialBlue);
			materialMgr.CreateMaterial(&pMaterial, materialId);
			loader.LoadMaterial(pAsset->GetPath(), *pMaterial);
			assetIdToMaterialId[pAsset->GetId()] = materialId;
		}
		{
			pAsset = assetMgr.GetAsset(materialOrange);
			materialMgr.CreateMaterial(&pMaterial, materialId);
			loader.LoadMaterial(pAsset->GetPath(), *pMaterial);
			assetIdToMaterialId[pAsset->GetId()] = materialId;
		}
		{
			pAsset = assetMgr.GetAsset(materialVertexColor);
			materialMgr.CreateMaterial(&pMaterial, materialId);
			loader.LoadMaterial(pAsset->GetPath(), *pMaterial);
			assetIdToMaterialId[pAsset->GetId()] = materialId;
		}

		//create a base plan with a scale as a root
		SceneTree* pSceneTree = level.GetSceneTree();
		Entity* pRootEntity = new Entity("/");
		pSceneTree->AddNode(pRootEntity, Os::Guid());

		Entity* pPlan = new Entity("plan");
		Component* pPlanTransform = CreateComponentTransform();
		float scale = 100;
		pPlanTransform->SetPropertyValue("Local", Core::Mat44f(
			Core::Vec4f(scale, 0, 0, 0),
			Core::Vec4f(0, scale, 0, 0),
			Core::Vec4f(0, 0, scale, 0),
			Core::Vec4f(0, 0, 0, 1)));

		Component* pPlanRendering = CreateComponentRendering();
		pPlanRendering->SetPropertyValue("Mesh", meshPlane);
		pPlanRendering->SetPropertyValue("Material", materialBlue);
		pPlan->AddComponent(pPlanTransform);
		pPlan->AddComponent(pPlanRendering);

		pSceneTree->AddNode(pPlan, pRootEntity->GetConstGuid());

		{
			Entity* pCube = new Entity("dummy1");
			pSceneTree->AddNode(pCube, pPlan->GetConstGuid());
		}
		{
			Entity* pCube = new Entity("dummy2");
			pSceneTree->AddNode(pCube, pPlan->GetConstGuid());
		}

		//first child is a cube
		Entity* pCube = new Entity("cube");
		Component* pCubeTransform = CreateComponentTransform();
		pCubeTransform->SetPropertyValue("Local", Core::Mat44f(Core::Vec4f(1, 0, 0, 0), Core::Vec4f(0, 1, 0, 0), Core::Vec4f(0, 0, 1, 0), Core::Vec4f(-5, 0, 0, 1)));

		Component* pCubeRendering = CreateComponentRendering();
		pCubeRendering->SetPropertyValue("Mesh", meshCube);
		pCubeRendering->SetPropertyValue("Material", materialOrange);

		pCube->AddComponent(pCubeTransform);
		pCube->AddComponent(pCubeRendering);
		pSceneTree->AddNode(pCube, pRootEntity->GetConstGuid());

		//second child, the torus
		Entity* pTorus = new Entity("torus");
		Component* pTorusTransform = CreateComponentTransform();
		pTorusTransform->SetPropertyValue("Local", Core::Mat44f(Core::Vec4f(1, 0, 0, 0), Core::Vec4f(0, 1, 0, 0), Core::Vec4f(0, 0, 1, 0), Core::Vec4f(5, 0, 0, 1)));

		Component* pTorusRendering = CreateComponentRendering();
		pTorusRendering->SetPropertyValue("Mesh", meshTorus);
		pTorusRendering->SetPropertyValue("Material", materialVertexColor);

		pTorus->AddComponent(pTorusTransform);
		pTorus->AddComponent(pTorusRendering);
		pSceneTree->AddNode(pTorus, pRootEntity->GetConstGuid());
	}

	LevelEditorModule::LevelEditorModule()
		: m_fovRad(DirectX::XMConvertToRadians(45.f))
	{
		m_pLevelMgr = new LevelMgr();
		m_pSelectionMgr = new SelectionMgr();
	}

	LevelEditorModule::~LevelEditorModule()
	{
		delete m_pSelectionMgr;
		delete m_pLevelMgr;
	}

	void LevelEditorModule::CreateEditor(Widgets::Widget* pParent)
	{
		CreateLevel(*m_pLevelMgr, m_assetIdToMeshId, m_assetIdToMaterialId);

		LevelEditorTab* pTab = new LevelEditorTab(pParent);
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

	void LevelEditorModule::AddNewEntity(Os::Guid& nodeGuid)
	{
		SceneTree* pSceneTree = m_pLevelMgr->GetSceneTree();

		Node* pParent = pSceneTree->GetRoot();

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

		pSceneTree->AddNode(pNewEntity, pParent->GetConstGuid());

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

	Core::CallbackId LevelEditorModule::OnAddEntity(const OnAddEntityEvent::Callback& callback)
	{
		return m_onAddEntity.Connect(callback);
	}

	void LevelEditorModule::RemoveOnAddEntity(Core::CallbackId id)
	{
		m_onAddEntity.Disconnect(id);
	}

	Core::CallbackId LevelEditorModule::OnDeleteEntity(const OnDeleteEntityEvent::Callback& callback)
	{
		return m_onDeleteEntity.Connect(callback);
	}

	void LevelEditorModule::RemoveOnDeleteEntity(Core::CallbackId id)
	{
		m_onDeleteEntity.Disconnect(id);
	}

	Core::CallbackId LevelEditorModule::OnRenameEntity(const OnRenameEntityEvent::Callback& callback)
	{
		return m_onRenameEntity.Connect(callback);
	}

	void LevelEditorModule::RemoveOnRenameEntity(Core::CallbackId id)
	{
		m_onRenameEntity.Disconnect(id);
	}
}
