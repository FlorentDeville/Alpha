/********************************************************************/
/* © 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/LevelEditor/LevelEditorModule.h"

#include "Core/Math/Vec4f.h"

#include "Editors/LevelEditor/Component.h"
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

	void CreateLevel(Level& level, std::map<Systems::AssetId, Rendering::MeshId>& assetIdToMeshId, std::map<Systems::AssetId, Rendering::MaterialId>& assetIdToMaterialId)
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
		: m_level()
		, m_fovRad(DirectX::XMConvertToRadians(45.f))
	{
		m_pSelectionMgr = new SelectionMgr();
	}

	LevelEditorModule::~LevelEditorModule()
	{
		delete m_pSelectionMgr;
	}

	void LevelEditorModule::CreateEditor(Widgets::Widget* pParent)
	{
		CreateLevel(m_level, m_assetIdToMeshId, m_assetIdToMaterialId);

		LevelEditorTab* pTab = new LevelEditorTab(pParent);
	}

	const Level& LevelEditorModule::GetConstLevel() const
	{
		return m_level;
	}

	Level& LevelEditorModule::GetLevel()
	{
		return m_level;
	}

	void LevelEditorModule::AddNewEntity(const std::string& name)
	{
		SceneTree* pSceneTree = m_level.GetSceneTree();

		Node* pParent = pSceneTree->GetRoot();

		std::string entityName = "DEFAULT";
		if (!name.empty())
			entityName = name;

		Entity* pPlan = new Entity(entityName);
		Component* pPlanTransform = CreateComponentTransform();
		pPlanTransform->SetPropertyValue("Local", Core::Mat44f(
			Core::Vec4f(1, 0, 0, 0),
			Core::Vec4f(0, 1, 0, 0),
			Core::Vec4f(0, 0, 1, 0),
			Core::Vec4f(0, 0, 0, 1)));

		Component* pPlanRendering = CreateComponentRendering();
		/*pPlanRendering->SetPropertyValue("Mesh", meshPlane);
		pPlanRendering->SetPropertyValue("Material", materialBlue);*/
		pPlan->AddComponent(pPlanTransform);
		pPlan->AddComponent(pPlanRendering);

		pSceneTree->AddNode(pPlan, pParent->GetConstGuid());
	}

	void LevelEditorModule::DeleteEntity(Node* pNode)
	{
		m_level.GetSceneTree()->DeleteNode(pNode->GetConstGuid());
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
}
