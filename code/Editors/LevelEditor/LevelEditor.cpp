/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/LevelEditor/LevelEditor.h"

#include "Core/Math/Mat44f.h"
#include "Core/Tree/Tree.h"

#include "Editors/LevelEditor/Component.h"
#include "Editors/LevelEditor/Entity.h"

#include "Editors/Widgets/Entity/EntityModel.h"
#include "Editors/Widgets/Entity/EntityWidget.h"
#include "Editors/Widgets/Tree/LevelTreeModel.h"
#include "Editors/Widgets/Tree/TreeWidget.h"

#include "Inputs/InputMgr.h"

#include "Rendering/Material/MaterialMgr.h"
#include "Rendering/Mesh/MeshMgr.h"
#include "Rendering/RenderModule.h"
#include "Rendering/RenderTargets/RenderTarget.h"

#include "Systems/Assets/Asset.h"
#include "Systems/Assets/AssetMgr.h"
#include "Systems/Loader.h"

#include "Widgets/Container.h"
#include "Widgets/Button.h"
#include "Widgets/Label.h"
#include "Widgets/Layout.h"
#include "Widgets/ModalWindow.h"
#include "Widgets/SplitVertical.h"
#include "Widgets/Tab.h"
#include "Widgets/TabContainer.h"
#include "Widgets/TextBox.h"
#include "Widgets/Viewport.h"
#include "Widgets/WidgetMgr.h"

#pragma optimize("", off)

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
			
			if (meshAssetId != Systems::AssetId::INVALID && materialAssetId != Systems::AssetId::INVALID)
			{
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
		Entity* pRootEntity = new Entity("/");
		root.SetContent(pRootEntity);

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

		Core::TreeNode<Entity*>& planNode = level.AddEntity(pPlan, root);

		{
			Entity* pCube = new Entity("dummy1");
			level.AddEntity(pCube, planNode);
		}
		{
			Entity* pCube = new Entity("dummy2");
			level.AddEntity(pCube, planNode);
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
		level.AddEntity(pCube, root);

		//second child, the torus
		Entity* pTorus = new Entity("torus");
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
		, m_pEntityModel(nullptr)
		, m_pEntityWidget(nullptr)
		, m_pEntityNameLabel(nullptr)
		, m_pTreeWidget(nullptr)
		, m_pLevelTreeModel(nullptr)
	{
		m_cameraPosition = DirectX::XMVectorSet(0, 10, -10, 1);
		m_cameraEulerAngle = DirectX::XMVectorSet(3.14f / 4.f, 0, 0, 0);
		
		DirectX::XMMATRIX YRotation = DirectX::XMMatrixRotationY(m_cameraEulerAngle.m128_f32[1]);
		DirectX::XMMATRIX XRotation = DirectX::XMMatrixRotationX(m_cameraEulerAngle.m128_f32[0]);
		m_cameraRotation = DirectX::XMMatrixMultiply(XRotation, YRotation);

		DirectX::XMMATRIX translationMatrix = DirectX::XMMatrixTranslationFromVector(m_cameraPosition);
		m_cameraTransform = DirectX::XMMatrixMultiply(m_cameraRotation, translationMatrix);
	}

	LevelEditor::~LevelEditor()
	{
		delete m_pRenderTarget;
		delete m_pEntityModel;
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
		pSplit->SetSizeStyle(Widgets::Widget::STRETCH);
		pSplit->SetRightPanelWidth(400);
		pSplit->SetResizePolicy(Widgets::SplitVertical::KeepRightSize);
		pViewportTab->AddWidget(pSplit);

		Widgets::Viewport* pViewport = new Widgets::Viewport();
		pViewport->SetSizeStyle(Widgets::Widget::STRETCH);
		pViewport->OnGetFocus([this]() -> bool { m_enableViewportControl = true; return true; });
		pViewport->OnLoseFocus([this]() -> bool { m_enableViewportControl = false; return true; });
		pViewport->OnGetRenderTargetTexture([this]() -> Rendering::TextureId { return RenderModule::Get().GetRenderTargetTextureId(m_pRenderTarget); });

		//create split between viewport and left panel
		Widgets::SplitVertical* pLeftSplit = new Widgets::SplitVertical();
		pLeftSplit->SetSizeStyle(Widgets::Widget::STRETCH);
		pLeftSplit->SetLeftPanelWidth(200);
		pLeftSplit->SetResizePolicy(Widgets::SplitVertical::KeepLeftSize);
		pLeftSplit->AddRightPanel(pViewport);
		

		pSplit->AddLeftPanel(pLeftSplit);

		Widgets::TabContainer* pTabContainer = dynamic_cast<Widgets::TabContainer*>(pParent);
		if (pTabContainer)
		{
			pTabContainer->AddTab("Level", pViewportTab);
		}
		else
		{
			pParent->AddWidget(pViewportTab);
		}

		CreateEntityPropertyGrid(pSplit);
		CreateSceneTreeViewer(pLeftSplit);
	}

	void LevelEditor::Update()
	{
		if (!m_enableViewportControl)
			return;

		bool updateCameraTransform = false;
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

			if (delta.x != 0 || delta.y != 0)
			{
				const float ROTATION_SPEED = 0.003f;
				DirectX::XMVECTOR eulerRotation = DirectX::XMVectorSet(-static_cast<float>(delta.y) * ROTATION_SPEED, -static_cast<float>(delta.x) * ROTATION_SPEED, 0, 0);
				m_cameraEulerAngle += eulerRotation;

				DirectX::XMMATRIX YRotation = DirectX::XMMatrixRotationY(m_cameraEulerAngle.m128_f32[1]);
				DirectX::XMMATRIX XRotation = DirectX::XMMatrixRotationX(m_cameraEulerAngle.m128_f32[0]);

				m_cameraRotation = DirectX::XMMatrixMultiply(XRotation, YRotation);
				m_mousePreviousPos = mousePosition;
				updateCameraTransform = true;
			}
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
			DirectX::XMVECTOR zAxis = m_cameraRotation.r[2];
			m_cameraPosition = DirectX::XMVectorAdd(m_cameraPosition, zAxis * mouseWheelDistance * CAMERA_DISTANCE_SPEED);
			updateCameraTransform = true;
		}

		const float TRANSLATION_SPEED = 0.5f;
		if (inputs.IsKeyPressed('W')) //forward
		{
			DirectX::XMVECTOR zAxis = m_cameraRotation.r[2];
			m_cameraPosition = DirectX::XMVectorAdd(m_cameraPosition, zAxis * TRANSLATION_SPEED);
			updateCameraTransform = true;
		}
		if (inputs.IsKeyPressed('S')) //backward
		{
			DirectX::XMVECTOR zAxis = m_cameraRotation.r[2];
			m_cameraPosition = DirectX::XMVectorAdd(m_cameraPosition, -zAxis * TRANSLATION_SPEED);
			updateCameraTransform = true;
		}
		if (inputs.IsKeyPressed('A')) //left
		{
			DirectX::XMVECTOR zAxis = m_cameraRotation.r[0];
			m_cameraPosition = DirectX::XMVectorAdd(m_cameraPosition, -zAxis * TRANSLATION_SPEED);
			updateCameraTransform = true;
		}
		if (inputs.IsKeyPressed('D')) //right
		{
			DirectX::XMVECTOR zAxis = m_cameraRotation.r[0];
			m_cameraPosition = DirectX::XMVectorAdd(m_cameraPosition, zAxis * TRANSLATION_SPEED);
			updateCameraTransform = true;
		}
		if (inputs.IsKeyPressed('Q')) //up
		{
			DirectX::XMVECTOR zAxis = m_cameraRotation.r[1];
			m_cameraPosition = DirectX::XMVectorAdd(m_cameraPosition, zAxis * TRANSLATION_SPEED);
			updateCameraTransform = true;
		}
		if (inputs.IsKeyPressed('E')) //down
		{
			DirectX::XMVECTOR zAxis = m_cameraRotation.r[1];
			m_cameraPosition = DirectX::XMVectorAdd(m_cameraPosition, -zAxis * TRANSLATION_SPEED);
			updateCameraTransform = true;
		}

		if (updateCameraTransform)
		{
			DirectX::XMMATRIX translationMatrix = DirectX::XMMatrixTranslationFromVector(m_cameraPosition);
			m_cameraTransform = DirectX::XMMatrixMultiply(m_cameraRotation, translationMatrix);
		}
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

	void LevelEditor::CreateEntityPropertyGrid(Widgets::SplitVertical* pSplit)
	{
		Widgets::Layout* pLayout = new Widgets::Layout(0, 0, 0, 0);
		pLayout->SetSizeStyle(Widgets::Widget::SIZE_STYLE::STRETCH);
		pLayout->SetDirection(Widgets::Layout::Vertical);

		pSplit->AddRightPanel(pLayout);

		m_pEntityNameLabel = new Widgets::Label();
		m_pEntityNameLabel->SetSize(DirectX::XMUINT2(0, 20));
		pLayout->AddWidget(m_pEntityNameLabel);

		m_pEntityModel = new EntityModel(new Entity(""));
		m_pEntityWidget = new EntityWidget();
		m_pEntityWidget->SetModel(m_pEntityModel);
		pLayout->AddWidget(m_pEntityWidget);
	}

	bool LevelEditor::OnTreeItemClicked(BaseModel* pModel, int rowId)
	{
		LevelTreeModel* pLevelTreeModel = static_cast<LevelTreeModel*>(pModel);
		Core::TreeNode<Entity*>* pNode = pLevelTreeModel->GetSource();
		Entity* pEntity = pNode->GetContent();

		delete m_pEntityModel;
		m_pEntityModel = new EntityModel(pEntity);
		m_pEntityWidget->SetModel(m_pEntityModel);
		Widgets::WidgetMgr::Get().RequestResize();

		m_pEntityNameLabel->SetText(pEntity->GetName());

		return true;
	}

	void LevelEditor::CreateSceneTreeViewer(Widgets::SplitVertical* pSplit)
	{
		Widgets::Layout* pLayout = new Widgets::Layout();
		pLayout->SetSizeStyle(Widgets::Widget::STRETCH);
		pLayout->SetDirection(Widgets::Layout::Vertical);
		pSplit->AddLeftPanel(pLayout);

		Widgets::Layout* pMenuLayout = new Widgets::Layout();
		pMenuLayout->SetSizeStyle(Widgets::Widget::HSIZE_STRETCH | Widgets::Widget::VSIZE_DEFAULT);
		pMenuLayout->SetSize(DirectX::XMUINT2(0, 20));
		pMenuLayout->SetDirection(Widgets::Layout::Horizontal);
		pLayout->AddWidget(pMenuLayout);

		const int BUTTON_SIZE = 20;
		//add entity button
		{
			Widgets::Button* pButton = new Widgets::Button(BUTTON_SIZE, BUTTON_SIZE, 0, 0);
			pMenuLayout->AddWidget(pButton);
			Widgets::Label* pButtonLabel = new Widgets::Label(0, 0, 1, "+");
			pButtonLabel->SetX(5);
			pButton->AddWidget(pButtonLabel);
			pButton->OnClick(std::bind(&LevelEditor::OnAddEntityClicked, this));
		}

		//remove entity
		{
			Widgets::Button* pButton = new Widgets::Button(BUTTON_SIZE, BUTTON_SIZE, 0, 0);
			pMenuLayout->AddWidget(pButton);
			Widgets::Label* pButtonLabel = new Widgets::Label(0, 0, 1, "-");
			pButtonLabel->SetX(7);
			pButton->AddWidget(pButtonLabel);
			pButton->OnClick(std::bind(&LevelEditor::OnRemoveEntityClicked, this));
		}

		////rename entity
		//{
		//	Widgets::Button* pButton = new Widgets::Button(BUTTON_SIZE, BUTTON_SIZE, 0, 0);
		//	pMenuLayout->AddWidget(pButton);
		//	Widgets::Label* pButtonLabel = new Widgets::Label(0, 0, 1, "RENAME");
		//	pButtonLabel->SetX(2);
		//	pButton->AddWidget(pButtonLabel);
		//}

		//separator
		Widgets::Container* pSeparator = new Widgets::Container(0, 2);
		pSeparator->SetSizeStyle(Widgets::Widget::HSIZE_STRETCH | Widgets::Widget::VSIZE_DEFAULT);
		pSeparator->GetDefaultStyle().SetBackgroundColor(DirectX::XMVectorSet(0.18f, 0.18f, 0.18f, 1));
		pLayout->AddWidget(pSeparator);

		m_pLevelTreeModel = new LevelTreeModel(&m_level.GetRoot());
		m_pTreeWidget = new TreeWidget();
		m_pTreeWidget->SetModel(m_pLevelTreeModel);

		pLayout->AddWidget(m_pTreeWidget);

		m_pTreeWidget->OnItemClicked(std::bind(&LevelEditor::OnTreeItemClicked, this, std::placeholders::_1, std::placeholders::_2));
	}

	bool LevelEditor::OnAddEntityClicked()
	{
		//show a modal window to enter the entity name
		Widgets::ModalWindow* pWindow = new Widgets::ModalWindow("Entity Name");
		pWindow->SetSize(DirectX::XMUINT2(500, 70));
		pWindow->SetSizeStyle(Widgets::Widget::DEFAULT);
		pWindow->SetPositionStyle(Widgets::Widget::HPOSITION_STYLE::CENTER, Widgets::Widget::VPOSITION_STYLE::MIDDLE);

		//vlayout
		Widgets::Layout* pVLayout = new Widgets::Layout();
		pVLayout->SetDirection(Widgets::Layout::Vertical);
		pVLayout->SetSizeStyle(Widgets::Widget::STRETCH);
		pWindow->AddWidget(pVLayout);

		//text box for the name of the entity
		Widgets::TextBox* pNameTextBox = new Widgets::TextBox();
		pNameTextBox->SetSize(DirectX::XMUINT2(0, 20));
		pNameTextBox->SetSizeStyle(Widgets::Widget::HSIZE_STRETCH | Widgets::Widget::VSIZE_DEFAULT);

		/*pNameTextBox->OnValidate([this](const std::string& value) -> bool 
			{ 
				AddNewEntity(value);
				Widgets::WidgetMgr::Get().CloseModalWindow();
				return true; 
			});*/
		pVLayout->AddWidget(pNameTextBox);
		
		//button ok escape
		Widgets::Layout* pHLayout = new Widgets::Layout();
		pHLayout->SetDirection(Widgets::Layout::Horizontal);
		pHLayout->SetSizeStyle(Widgets::Widget::STRETCH);
		pVLayout->AddWidget(pHLayout);

		Widgets::Button* pOkButton = new Widgets::Button(250, 25, 0, 0);
		Widgets::Label* pOkLabel = new Widgets::Label(0, 0, 1, "OK");
		pOkLabel->SetSizeStyle(Widgets::Widget::FIT);
		pOkLabel->SetPositionStyle(Widgets::Widget::HPOSITION_STYLE::CENTER, Widgets::Widget::VPOSITION_STYLE::MIDDLE);
		pOkButton->AddWidget(pOkLabel);
		pOkButton->SetSizeStyle(Widgets::Widget::HSIZE_DEFAULT | Widgets::Widget::VSIZE_STRETCH);
		pOkButton->OnClick([this, pNameTextBox](int, int) -> bool
			{
				const std::string& text = pNameTextBox->GetText();
				AddNewEntity(text);

				delete m_pLevelTreeModel;
				m_pLevelTreeModel = new LevelTreeModel(&m_level.GetRoot());
				m_pTreeWidget->SetModel(m_pLevelTreeModel);
				Widgets::WidgetMgr::Get().CloseModalWindow();
				return true;
			});
		pHLayout->AddWidget(pOkButton);

		Widgets::Button* pCancelButton = new Widgets::Button(250, 25, 0, 0);
		Widgets::Label* pCancelLabel = new Widgets::Label(0, 0, 1, "CANCEL");
		pCancelLabel->SetSizeStyle(Widgets::Widget::FIT);
		pCancelLabel->SetPositionStyle(Widgets::Widget::HPOSITION_STYLE::CENTER, Widgets::Widget::VPOSITION_STYLE::MIDDLE);
		pCancelButton->AddWidget(pCancelLabel);
		pCancelButton->SetSizeStyle(Widgets::Widget::HSIZE_DEFAULT | Widgets::Widget::VSIZE_STRETCH);
		pCancelButton->OnClick([](int, int) -> bool { Widgets::WidgetMgr::Get().CloseModalWindow(); return true; });
		pHLayout->AddWidget(pCancelButton);

		Widgets::WidgetMgr::Get().OpenModalWindow(pWindow);
		Widgets::WidgetMgr::Get().SetFocus(pNameTextBox);

		return true;
	}

	bool LevelEditor::OnRemoveEntityClicked()
	{
		//find the currently selected entity
		BaseModel* pBaseModel = m_pTreeWidget->GetSelectedItemModel();
		if (!pBaseModel)
			return false;

		LevelTreeModel* pTreeModel = static_cast<LevelTreeModel*>(pBaseModel);
		Core::TreeNode<Entity*>* pNode = pTreeModel->GetSource();
		if (!pNode)
			return false;

		const Entity* pSelectedEntity = pNode->GetContent();
		if (!pSelectedEntity)
			return false;

		//show a modal window to enter the entity name
		std::string title = "Do you really want to delete entity " + pSelectedEntity->GetName() + " ?";
		Widgets::ModalWindow* pWindow = new Widgets::ModalWindow(title);
		pWindow->SetSize(DirectX::XMUINT2(500, 50));
		pWindow->SetSizeStyle(Widgets::Widget::DEFAULT);
		pWindow->SetPositionStyle(Widgets::Widget::HPOSITION_STYLE::CENTER, Widgets::Widget::VPOSITION_STYLE::MIDDLE);

		//button ok escape
		Widgets::Layout* pHLayout = new Widgets::Layout();
		pHLayout->SetDirection(Widgets::Layout::Horizontal);
		pHLayout->SetSizeStyle(Widgets::Widget::STRETCH);
		pWindow->AddWidget(pHLayout);

		Widgets::Button* pOkButton = new Widgets::Button(250, 25, 0, 0);
		Widgets::Label* pOkLabel = new Widgets::Label(0, 0, 1, "OK");
		pOkLabel->SetSizeStyle(Widgets::Widget::FIT);
		pOkLabel->SetPositionStyle(Widgets::Widget::HPOSITION_STYLE::CENTER, Widgets::Widget::VPOSITION_STYLE::MIDDLE);
		pOkButton->AddWidget(pOkLabel);
		pOkButton->SetSizeStyle(Widgets::Widget::HSIZE_DEFAULT | Widgets::Widget::VSIZE_STRETCH);
		pOkButton->OnClick([this, pNode](int, int) -> bool
			{
				DeleteEntity(pNode);
				delete m_pLevelTreeModel;
				m_pLevelTreeModel = new LevelTreeModel(&m_level.GetRoot());
				m_pTreeWidget->SetModel(m_pLevelTreeModel);
				Widgets::WidgetMgr::Get().CloseModalWindow();
				return true;
			});
		pHLayout->AddWidget(pOkButton);

		Widgets::Button* pCancelButton = new Widgets::Button(250, 25, 0, 0);
		Widgets::Label* pCancelLabel = new Widgets::Label(0, 0, 1, "CANCEL");
		pCancelLabel->SetSizeStyle(Widgets::Widget::FIT);
		pCancelLabel->SetPositionStyle(Widgets::Widget::HPOSITION_STYLE::CENTER, Widgets::Widget::VPOSITION_STYLE::MIDDLE);
		pCancelButton->AddWidget(pCancelLabel);
		pCancelButton->SetSizeStyle(Widgets::Widget::HSIZE_DEFAULT | Widgets::Widget::VSIZE_STRETCH);
		pCancelButton->OnClick([](int, int) -> bool { Widgets::WidgetMgr::Get().CloseModalWindow(); return true; });
		pHLayout->AddWidget(pCancelButton);

		Widgets::WidgetMgr::Get().OpenModalWindow(pWindow);

		return true;
	}

	void LevelEditor::AddNewEntity(const std::string& name)
	{
		Core::TreeNode<Entity*>& parent = m_level.GetRoot();

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

		Core::TreeNode<Entity*>& planNode = m_level.AddEntity(pPlan, parent);
	}

	void LevelEditor::DeleteEntity(Core::TreeNode<Entity*>* pNode)
	{
		Core::TreeNode<Entity*>* pParent = pNode->GetParent();
		assert(pParent);

		m_level.DeleteEntity(*pNode);
	}
}
