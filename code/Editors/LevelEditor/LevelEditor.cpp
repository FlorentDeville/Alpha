/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/LevelEditor/LevelEditor.h"

#include "Core/Math/Mat44f.h"
#include "Core/Tree/Tree.h"

#include "Editors/LevelEditor/Component.h"
#include "Editors/LevelEditor/GizmoWidget.h"
#include "Editors/LevelEditor/LevelEditorViewportWidget.h"
#include "Editors/LevelEditor/SceneTree/Entity.h"
#include "Editors/LevelEditor/SceneTree/SceneTree.h"

#include "Editors/Widgets/Entity/EntityModel.h"
#include "Editors/Widgets/Entity/EntityWidget.h"
#include "Editors/Widgets/Tree/LevelTreeModel.h"
#include "Editors/Widgets/Tree/TreeWidget.h"

#include "Inputs/InputMgr.h"

#include "Rendering/Camera.h"
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
#include "Widgets/Viewport_v2.h"
#include "Widgets/WidgetMgr.h"

//#pragma optimize("", off)

//needed for the operator* between vectors and floats.
using namespace DirectX;

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

	LevelEditor::LevelEditor()
		: m_level()
		, m_pEntityModel(nullptr)
		, m_pEntityWidget(nullptr)
		, m_pEntityNameLabel(nullptr)
		, m_pTreeWidget(nullptr)
		, m_pLevelTreeModel(nullptr)
	{
		m_pGizmoWidget = new GizmoWidget();
	}

	LevelEditor::~LevelEditor()
	{
		delete m_pEntityModel;
	}

	void LevelEditor::CreateEditor(Widgets::Widget* pParent)
	{
		CreateLevel(m_level, m_assetIdToMeshId, m_assetIdToMaterialId);

		//create the render target
		int width = 1280;
		int height = 720;

		//create the widgets
		Widgets::Tab* pViewportTab = new Widgets::Tab();

		//create the split between viewport and right panel
		Widgets::SplitVertical* pSplit = new Widgets::SplitVertical();
		pSplit->SetSizeStyle(Widgets::Widget::STRETCH);
		pSplit->SetRightPanelWidth(400);
		pSplit->SetResizePolicy(Widgets::SplitVertical::KeepRightSize);
		pViewportTab->AddWidget(pSplit);

		LevelEditorViewportWidget* pViewport = new LevelEditorViewportWidget(width, height);
		pViewport->SetSizeStyle(Widgets::Widget::STRETCH);
		pViewport->OnGetFocus([pViewport]() -> bool { pViewport->SetEnableViewportControl(true); return true; });
		pViewport->OnLoseFocus([pViewport]() -> bool { pViewport->SetEnableViewportControl(false); return true; });

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
	{}

	void LevelEditor::Render()
	{}

	const Level& LevelEditor::GetConstLevel() const
	{
		return m_level;
	}

	Level& LevelEditor::GetLevel()
	{
		return m_level;
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
		Node* pNode = pLevelTreeModel->GetSource();
		Entity* pEntity = pNode->ToEntity();

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

		m_pLevelTreeModel = new LevelTreeModel(m_level.GetSceneTree()->GetRoot());
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
				m_pLevelTreeModel = new LevelTreeModel(m_level.GetSceneTree()->GetRoot());
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
		Node* pNode = pTreeModel->GetSource();
		if (!pNode)
			return false;

		const Entity* pSelectedEntity = pNode->ToConstEntity();
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
				m_pLevelTreeModel = new LevelTreeModel(m_level.GetSceneTree()->GetRoot());
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

	void LevelEditor::DeleteEntity(Node* pNode)
	{
		m_level.GetSceneTree()->DeleteNode(pNode->GetConstGuid());
	}
}
