/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/LevelEditor/LevelEditor.h"

#include "Core/Math/Mat44f.h"
#include "Core/Tree/Tree.h"

#include "Editors/LevelEditor/Component.h"
#include "Editors/LevelEditor/Entity.h"
#include "Editors/Widgets/AssetId/AssetIdModel.h"
#include "Editors/Widgets/AssetId/AssetIdWidget.h"

#include "Inputs/InputMgr.h"

#include "Rendering/Material/MaterialMgr.h"
#include "Rendering/Mesh/MeshMgr.h"
#include "Rendering/RenderModule.h"
#include "Rendering/RenderTargets/RenderTarget.h"

#include "Systems/Assets/Asset.h"
#include "Systems/Assets/AssetMgr.h"
#include "Systems/Loader.h"

#include "Widgets/Layout.h"
#include "Widgets/SplitVertical.h"
#include "Widgets/Tab.h"
#include "Widgets/TabContainer.h"
#include "Widgets/Viewport.h"

//needed for the operator* between vectors and floats.
using namespace DirectX;

namespace Editors
{
	void RenderTreeNodeRecursive(const Core::TreeNode<Entity*>* pNode, const DirectX::XMMATRIX& parent, const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& proj,
		const std::map<Systems::AssetId, Rendering::MeshId>& assetIdToMeshId, const std::map<Systems::AssetId, Rendering::MaterialId>& assetIdToMaterialId)
	{
		DirectX::XMMATRIX world = parent;

		const Entity* pEntity = pNode->GetContent();
		const Component* pTransformComponent = pEntity->GetComponent("Transform");
		if (pTransformComponent)
		{
			Core::Mat44f local;
			pTransformComponent->GetPropertyValue("Local", local);

			DirectX::XMMATRIX dxLocal(
				local.GetX().GetX(), local.GetX().GetY(), local.GetX().GetZ(), local.GetX().GetW(),
				local.GetY().GetX(), local.GetY().GetY(), local.GetY().GetZ(), local.GetY().GetW(),
				local.GetZ().GetX(), local.GetZ().GetY(), local.GetZ().GetZ(), local.GetZ().GetW(),
				local.GetT().GetX(), local.GetT().GetY(), local.GetT().GetZ(), local.GetT().GetW()
			);

			world = dxLocal * world;
		}

		const Component* pRenderingComponent = pEntity->GetComponent("Rendering");
		if (pRenderingComponent)
		{
			Systems::AssetId meshAssetId;
			Systems::AssetId materialAssetId;
			pRenderingComponent->GetPropertyValue("Mesh", meshAssetId);
			pRenderingComponent->GetPropertyValue("Material", materialAssetId);
			
			Rendering::MeshId meshId = assetIdToMeshId.find(meshAssetId)->second;
			Rendering::MaterialId materialId = assetIdToMaterialId.find(materialAssetId)->second;

			DirectX::XMMATRIX mvpMatrix = DirectX::XMMatrixMultiply(world, view);
			mvpMatrix = DirectX::XMMatrixMultiply(mvpMatrix, proj);

			RenderModule& renderer = RenderModule::Get();

			const Rendering::Material* pMaterial = Rendering::MaterialMgr::Get().GetMaterial(materialId);
			renderer.BindMaterial(*pMaterial, mvpMatrix);

			const Rendering::Mesh* pMesh = Rendering::MeshMgr::Get().GetMesh(meshId);
			renderer.RenderMesh(*pMesh);

		}

		//recursive call to children
		const std::vector<Core::TreeNode<Entity*>*>& children = pNode->GetChildren();
		for (const Core::TreeNode<Entity*>* pChild : children)
		{
			RenderTreeNodeRecursive(pChild, world, view, proj, assetIdToMeshId, assetIdToMaterialId);
		}
	}

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

	void AddAssetIdWidgetRecursive(Widgets::Layout* pLayout, const Core::TreeNode<Entity*>* pNode)
	{
		const Editors::Entity* pEntity = pNode->GetContent();
		if (pEntity)
		{
			const Editors::Component* pComponent = pEntity->GetComponent("Rendering");
			if (pComponent)
			{
				Systems::AssetId meshAssetId;
				Systems::AssetId materialAssetId;
				pComponent->GetPropertyValue("Mesh", meshAssetId);
				pComponent->GetPropertyValue("Material", materialAssetId);

				int ITEM_HEIGHT = 15;
				{
					Widgets::Layout* pItemLayout = new Widgets::Layout(0, ITEM_HEIGHT, 0, 0);
					pItemLayout->SetDirection(Widgets::Layout::Horizontal);
					pItemLayout->SetSizeStyle(Widgets::Widget::HSIZE_STRETCH);

					Widgets::Label* pLabel = new Widgets::Label();
					pLabel->SetText("Mesh Id");
					pLabel->SetSize(DirectX::XMUINT2(100, ITEM_HEIGHT));
					pLabel->SetSizeStyle(0);
					pItemLayout->AddWidget(pLabel);

					AssetIdWidget* pAssetIdWidget = new AssetIdWidget(0, 0, 1);
					pAssetIdWidget->SetModel(new AssetIdModel(meshAssetId));
					pItemLayout->AddWidget(pAssetIdWidget);
					pLayout->AddWidget(pItemLayout);
				}
				{
					Widgets::Layout* pItemLayout = new Widgets::Layout(0, ITEM_HEIGHT, 0, 0);
					pItemLayout->SetDirection(Widgets::Layout::Horizontal);
					pItemLayout->SetSizeStyle(Widgets::Widget::HSIZE_STRETCH);

					Widgets::Label* pLabel = new Widgets::Label();
					pLabel->SetText("Material Id");
					pLabel->SetSize(DirectX::XMUINT2(100, ITEM_HEIGHT));
					pLabel->SetSizeStyle(0);
					pItemLayout->AddWidget(pLabel);
					
					AssetIdWidget* pAssetIdWidget = new AssetIdWidget(0, 0, 1);
					pAssetIdWidget->SetModel(new AssetIdModel(materialAssetId));
					pItemLayout->AddWidget(pAssetIdWidget);
					pLayout->AddWidget(pItemLayout);
				}
			}
		}

		const std::vector<Core::TreeNode<Entity*>*>& children = pNode->GetChildren();
		for (const Core::TreeNode<Entity*>* pChild : children)
		{
			AddAssetIdWidgetRecursive(pLayout, pChild);
		}
	}

	void CreateLeftPanel(const Core::TreeNode<Entity*>* pNode, Widgets::SplitVertical* pSplit)
	{
		Widgets::Layout* pLayout = new Widgets::Layout(0, 0, 0, 0);
		pLayout->SetSizeStyle(Widgets::Widget::SIZE_STYLE::HSIZE_STRETCH | Widgets::Widget::SIZE_STYLE::VSIZE_STRETCH);
		pLayout->SetDirection(Widgets::Layout::Vertical);

		pSplit->AddRightPanel(pLayout);

		AddAssetIdWidgetRecursive(pLayout, pNode);
		//pLayout->AddWidget(new Widgets::Label(0, 0, 1, "Test"));
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
		Core::TreeNode<Entity*>& root = level.GetRoot();

		Entity* pPlan = new Entity();
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

		Core::TreeNode<Entity*>& planNode = level.AddEntity(pPlan, root);

		//first child is a cube
		Entity* pCube = new Entity();
		Component* pCubeTransform = CreateComponentTransform();
		pCubeTransform->SetPropertyValue("Local", Core::Mat44f(Core::Vec4f(1, 0, 0, 0), Core::Vec4f(0, 1, 0, 0), Core::Vec4f(0, 0, 1, 0), Core::Vec4f(-5, 0, 0, 1)));

		Component* pCubeRendering = CreateComponentRendering();
		pCubeRendering->SetPropertyValue("Mesh", meshCube);
		pCubeRendering->SetPropertyValue("Material", materialOrange);

		pCube->AddComponent(pCubeTransform);
		pCube->AddComponent(pCubeRendering);
		level.AddEntity(pCube, root);

		//second child, the torus
		Entity* pTorus = new Entity();
		Component* pTorusTransform = CreateComponentTransform();
		pTorusTransform->SetPropertyValue("Local", Core::Mat44f(Core::Vec4f(1, 0, 0, 0), Core::Vec4f(0, 1, 0, 0), Core::Vec4f(0, 0, 1, 0), Core::Vec4f(5, 0, 0, 1)));

		Component* pTorusRendering = CreateComponentRendering();
		pTorusRendering->SetPropertyValue("Mesh", meshTorus);
		pTorusRendering->SetPropertyValue("Material", materialVertexColor);

		pTorus->AddComponent(pTorusTransform);
		pTorus->AddComponent(pTorusRendering);
		level.AddEntity(pTorus, root);
	}

	LevelEditor::LevelEditor()
		: m_pRenderTarget(nullptr)
		, m_aspectRatio(0.f)
		, m_level()
		, m_enableViewportControl(false)
		, m_firstFrameMouseDown(true)
		, m_mousePreviousPos()
		, m_padding()
		, m_cameraTransform(DirectX::XMMatrixIdentity())
	{
		DirectX::XMVECTOR cameraPosition = DirectX::XMVectorSet(0, 10, -10, 1);
		DirectX::XMMATRIX cameraView = DirectX::XMMatrixLookAtLH(cameraPosition, DirectX::XMVectorSet(0, 0, 0, 1), DirectX::XMVectorSet(0, 1, 0, 0));
		cameraView.r[3] = DirectX::XMVectorSet(0, 0, 0, 1);

		m_cameraTransform = DirectX::XMMatrixTranspose(cameraView);
		m_cameraTransform.r[3] = cameraPosition;
	}

	LevelEditor::~LevelEditor()
	{
		delete m_pRenderTarget;
	}

	void LevelEditor::CreateEditor(Widgets::Widget* pParent)
	{
		CreateLevel(m_level, m_assetIdToMeshId, m_assetIdToMaterialId);

		//create the render target
		int width = 1280;
		int height = 720;
		m_aspectRatio = width / static_cast<float>(height);
		m_pRenderTarget = RenderModule::Get().CreateRenderTarget(width, height);

		//create the widgets
		Widgets::Tab* pViewportTab = new Widgets::Tab();

		//create the split between viewport and right panel
		Widgets::SplitVertical* pSplit = new Widgets::SplitVertical();
		pSplit->SetSizeStyle(Widgets::Widget::HSIZE_STRETCH | Widgets::Widget::VSIZE_STRETCH);
		pSplit->SetLeftPanelWidth(800);
		pViewportTab->AddWidget(pSplit);

		Widgets::Viewport* pViewport = new Widgets::Viewport();
		pViewport->SetSizeStyle(Widgets::Widget::HSIZE_STRETCH | Widgets::Widget::VSIZE_STRETCH);
		pViewport->OnGetFocus([this]() -> bool { m_enableViewportControl = true; return true; });
		pViewport->OnLoseFocus([this]() -> bool { m_enableViewportControl = false; return true; });
		pViewport->OnGetRenderTargetTexture([this]() -> Rendering::TextureId { return RenderModule::Get().GetRenderTargetTextureId(m_pRenderTarget); });

		pSplit->AddLeftPanel(pViewport);

		Widgets::TabContainer* pTabContainer = dynamic_cast<Widgets::TabContainer*>(pParent);
		if (pTabContainer)
		{
			pTabContainer->AddTab("Level", pViewportTab);
		}
		else
		{
			pParent->AddWidget(pViewportTab);
		}

		CreateLeftPanel(&m_level.GetRoot(), pSplit);
	}

	void LevelEditor::Update()
	{
		if (!m_enableViewportControl)
			return;

		Inputs::InputMgr& inputs = Inputs::InputMgr::Get();
		if (inputs.IsMouseLeftButtonDown())
		{
			DirectX::XMUINT2 mousePosition;
			inputs.GetMousePosition(mousePosition.x, mousePosition.y);
			if (m_firstFrameMouseDown)
			{
				m_mousePreviousPos = mousePosition;
				m_firstFrameMouseDown = false;
			}

			DirectX::XMINT2 delta;
			delta.x = m_mousePreviousPos.x - mousePosition.x;
			delta.y = m_mousePreviousPos.y - mousePosition.y;

			const float ROTATION_SPEED = 0.001f;
			DirectX::XMVECTOR eulerRotation = DirectX::XMVectorSet(-static_cast<float>(delta.y) * ROTATION_SPEED, -static_cast<float>(delta.x) * ROTATION_SPEED, 0, 0);
			DirectX::XMMATRIX orientation = DirectX::XMMatrixRotationRollPitchYawFromVector(eulerRotation);
			m_cameraTransform = DirectX::XMMatrixMultiply(orientation, m_cameraTransform);
			m_mousePreviousPos = mousePosition;
		}
		else if (!m_firstFrameMouseDown)
		{
			m_firstFrameMouseDown = true;
		}

		int16_t mouseWheelDistance = inputs.GetMouseWheelDistance();	
		if (mouseWheelDistance != 0)
		{
			const float CAMERA_DISTANCE_SPEED = 0.05f;
			const float MIN_DISTANCE = 2;
			DirectX::XMVECTOR translation = DirectX::XMVectorSet(0, 0, 1, 1) * mouseWheelDistance * CAMERA_DISTANCE_SPEED;
			DirectX::XMMATRIX translationMatrix = DirectX::XMMatrixTranslationFromVector(translation);
			m_cameraTransform = DirectX::XMMatrixMultiply(translationMatrix, m_cameraTransform);
		}

		DirectX::XMVECTOR translation = DirectX::XMVectorSet(0, 0, 0, 1);
		const float TRANSLATION_SPEED = 0.5f;
		if (inputs.IsKeyPressed('W')) //forward
		{
			translation = DirectX::XMVectorAdd(translation, DirectX::XMVectorSet(0, 0, TRANSLATION_SPEED, 1));
		}
		if (inputs.IsKeyPressed('S')) //backward
		{
			translation = DirectX::XMVectorAdd(translation, DirectX::XMVectorSet(0, 0, -TRANSLATION_SPEED, 1));
		}
		if (inputs.IsKeyPressed('A')) //left
		{
			translation = DirectX::XMVectorAdd(translation, DirectX::XMVectorSet(-TRANSLATION_SPEED, 0, 0, 1));
		}
		if (inputs.IsKeyPressed('D')) //right
		{
			translation = DirectX::XMVectorAdd(translation, DirectX::XMVectorSet(TRANSLATION_SPEED, 0, 0, 1));
		}
		if (inputs.IsKeyPressed('Q')) //up
		{
			translation = DirectX::XMVectorAdd(translation, DirectX::XMVectorSet(0, TRANSLATION_SPEED, 0, 1));
		}
		if (inputs.IsKeyPressed('E')) //up
		{
			translation = DirectX::XMVectorAdd(translation, DirectX::XMVectorSet(0, -TRANSLATION_SPEED, 0, 1));
		}
		DirectX::XMMATRIX translationMatrix = DirectX::XMMatrixTranslationFromVector(translation);
		m_cameraTransform = DirectX::XMMatrixMultiply(translationMatrix, m_cameraTransform);
	}

	void LevelEditor::Render()
	{
		m_pRenderTarget->BeginScene();

		//view
		DirectX::XMVECTOR cameraUp = DirectX::XMVectorSet(0, 1, 0, 1);

		DirectX::XMVECTOR targetOffset = DirectX::XMVectorSet(0, 0, 1, 1);
		DirectX::XMVECTOR cameraLookAt = DirectX::XMVector4Transform(targetOffset, m_cameraTransform);

		DirectX::XMVECTOR cameraPosition = m_cameraTransform.r[3];

		DirectX::XMVECTOR cameraDirection = DirectX::XMVectorSubtract(cameraLookAt, cameraPosition);
		cameraDirection = DirectX::XMVector4Normalize(cameraDirection);

		DirectX::XMMATRIX view = DirectX::XMMatrixLookToLH(cameraPosition, cameraDirection, cameraUp);

		//projection
		const float fov = 45.f;
		float nearDistance = 0.1f;
		float fovRad = DirectX::XMConvertToRadians(fov);
		DirectX::XMMATRIX projection = DirectX::XMMatrixPerspectiveFovLH(fovRad, m_aspectRatio, nearDistance, 1000.0f);

		//loop through the tree
		const Core::TreeNode<Entity*>& root = m_level.GetRoot();

		DirectX::XMMATRIX world = DirectX::XMMatrixIdentity();

		const std::vector<Core::TreeNode<Entity*>*>& children = root.GetChildren();
		for (const Core::TreeNode<Entity*>* pNode : children)
		{
			RenderTreeNodeRecursive(pNode, world, view, projection, m_assetIdToMeshId, m_assetIdToMaterialId);
		}
		
		m_pRenderTarget->EndScene();
	}
}
