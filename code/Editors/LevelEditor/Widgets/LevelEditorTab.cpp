/********************************************************************/
/* © 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/LevelEditor/Widgets/LevelEditorTab.h"

#include "Editors/LevelEditor/LevelEditorModule.h"
#include "Editors/LevelEditor/LevelMgr.h"
#include "Editors/LevelEditor/SceneTree/Entity.h"
#include "Editors/LevelEditor/SceneTree/Node.h"
#include "Editors/LevelEditor/SceneTree/SceneTree.h"
#include "Editors/LevelEditor/SelectionMgr.h"
#include "Editors/LevelEditor/Widgets/LevelEditorViewportWidget.h"
#include "Editors/LevelEditor/Widgets/GizmoModel.h"
#include "Editors/LevelEditor/Widgets/GizmoWidget.h"

#include "Editors/Widgets/Dialog/AssetDialog.h"
#include "Editors/Widgets/Entity/EntityModel.h"
#include "Editors/Widgets/Entity/EntityWidget.h"
#include "Editors/Widgets/Tree/LevelTreeModel.h"
#include "Editors/Widgets/Tree/TreeWidget.h"

#include "Inputs/InputMgr.h"

#include "Rendering/RenderModule.h"

#include "Widgets/Container.h"
#include "Widgets/Button.h"
#include "Widgets/Label.h"
#include "Widgets/Layout.h"
#include "Widgets/Menu.h"
#include "Widgets/MenuBar.h"
#include "Widgets/MenuItem.h"
#include "Widgets/ModalWindow.h"
#include "Widgets/SplitVertical.h"
#include "Widgets/TabContainer.h"
#include "Widgets/TextBox.h"
#include "Widgets/Viewport.h"
#include "Widgets/WidgetMgr.h"
#include "Widgets/Widgets/Frame.h"

namespace Editors
{
	LevelEditorTab::LevelEditorTab(Widgets::Widget* pParent)
		: Widgets::Tab()
		, m_enableViewportControl(false)
		, m_pEntityNameLabel(nullptr)
		, m_pEntityModel(nullptr)
		, m_pEntityWidget(nullptr)
		, m_pLevelTreeModel(nullptr)
		, m_pSceneTreeFrame(nullptr)
		, m_pLeftSplit(nullptr)
		, m_cidOnSelectionCleared_EntityProperties()
	{
		//create the render target
		int width = 1280;
		int height = 720;

		Widgets::Layout* pInternalLayout = new Widgets::Layout();
		pInternalLayout->SetDirection(Widgets::Layout::Direction::Vertical);
		pInternalLayout->SetSizeStyle(Widgets::Widget::SIZE_STYLE::STRETCH);
		AddWidget(pInternalLayout);

		Widgets::MenuBar* pMenuBar = new Widgets::MenuBar();

		CreateMenuFile(pMenuBar);
		CreateMenuEdit(pMenuBar);
		CreateMenuTransformation(pMenuBar);
		CreateMenuWindows(pMenuBar);

		pInternalLayout->AddWidget(pMenuBar);

		//create the split between viewport and right panel
		m_pSplit = new Widgets::SplitVertical();
		m_pSplit->SetSizeStyle(Widgets::Widget::STRETCH);
		m_pSplit->SetRightPanelWidth(400);
		m_pSplit->SetResizePolicy(Widgets::SplitVertical::KeepRightSize);
		pInternalLayout->AddWidget(m_pSplit);

		m_pViewport = new LevelEditorViewportWidget(width, height);
		m_pViewport->SetSizeStyle(Widgets::Widget::STRETCH);
		m_pViewport->OnFocusGained([this](const Widgets::FocusEvent&) { m_pViewport->SetEnableViewportControl(true); });
		m_pViewport->OnFocusLost([this](const Widgets::FocusEvent&) { m_pViewport->SetEnableViewportControl(false); });
		m_pViewport->GetGizmoWidget()->SetEnable(false);
		m_pViewport->OnMouseEnter([this](const Widgets::MouseEvent& ev) -> bool { m_pViewport->SetFocus(); return true; });

		//create split between viewport and left panel
		m_pLeftSplit = new Widgets::SplitVertical();
		m_pLeftSplit->SetSizeStyle(Widgets::Widget::STRETCH);
		m_pLeftSplit->SetLeftPanelWidth(200);
		m_pLeftSplit->SetResizePolicy(Widgets::SplitVertical::KeepLeftSize);
		m_pLeftSplit->AddRightPanel(m_pViewport);


		m_pSplit->AddLeftPanel(m_pLeftSplit);

		if (pParent)
		{
			Widgets::TabContainer* pTabContainer = dynamic_cast<Widgets::TabContainer*>(pParent);
			if (pTabContainer)
			{
				pTabContainer->AddTab("Level", this);
			}
			else
			{
				pParent->AddWidget(this);
			}
		}

		CreateEntityPropertyGrid(m_pSplit);
		CreateSceneTreeViewer(m_pLeftSplit);

		//gizmo callback registration
		LevelEditorModule& levelEditorModule = LevelEditorModule::Get();
		SelectionMgr* pSelectionMgr = levelEditorModule.GetSelectionMgr();
		pSelectionMgr->OnClear([this]() { OnSelectionCleared_Gizmo(); });
		pSelectionMgr->OnItemAdded([this](const Os::Guid& nodeGuid) { OnAddedToSelection_Gizmo(nodeGuid); });
		pSelectionMgr->OnItemRemoved([this](const Os::Guid& nodeGuid) { OnRemovedFromSelection_Gizmo(nodeGuid); });
	}

	LevelEditorTab::~LevelEditorTab()
	{}

	void LevelEditorTab::CreateMenuFile(Widgets::MenuBar* pMenuBar)
	{
		Widgets::Menu* pEditMenu = pMenuBar->AddMenu("File");

		Widgets::MenuItem* pNewItem = pEditMenu->AddMenuItem("New");
		pNewItem->OnClick([this]() { LevelEditorModule::Get().NewLevel(); });

		Widgets::MenuItem* pLoadItem = pEditMenu->AddMenuItem("Load");
		pLoadItem->OnClick([this]() { OnClickFileMenu_LoadLevel(); });

		Widgets::MenuItem* pSaveItem = pEditMenu->AddMenuItem("Save");
		pSaveItem->SetShortcut("Ctrl+S");
		pSaveItem->OnClick([this]() { OnClickFileMenu_Save(); });

		Widgets::MenuItem* pSaveAsItem = pEditMenu->AddMenuItem("Save As...");
		pSaveAsItem->OnClick([this]() { OnClickFileMenu_SaveAs(); });

		Editors::LevelEditorModule& levelEditorModule = Editors::LevelEditorModule::Get();
		levelEditorModule.OnNewLevel([pSaveItem]() { pSaveItem->SetText("Save"); });
		levelEditorModule.OnLoadLevel([pSaveItem]() { pSaveItem->SetText("Save " + Editors::LevelEditorModule::Get().GetCurrentLoadedLevelName()); });
		levelEditorModule.OnSaveLevel([pSaveItem]() { pSaveItem->SetText("Save " + Editors::LevelEditorModule::Get().GetCurrentLoadedLevelName()); });
	}

	void LevelEditorTab::CreateMenuEdit(Widgets::MenuBar* pMenuBar)
	{
		Widgets::Menu* pEditMenu = pMenuBar->AddMenu("Edit");

		Widgets::MenuItem* pAddItem = pEditMenu->AddMenuItem("Add");
		pAddItem->OnClick([this]() { OnClickEditMenu_AddEntity(); });

		Widgets::MenuItem* pDeleteItem = pEditMenu->AddMenuItem("Delete");
		pDeleteItem->SetShortcut("Del");
		pDeleteItem->OnClick([this]() { OnClickEditMenu_DeleteEntity(); });

		Widgets::MenuItem* pRenameItem = pEditMenu->AddMenuItem("Rename...");
		pRenameItem->OnClick([this]() { OnClickEditMenu_RenameEntity(); });

		Widgets::MenuItem* pDuplicateItem = pEditMenu->AddMenuItem("Duplicate");
		pDuplicateItem->SetShortcut("Ctrl+D");
		pDuplicateItem->OnClick([this]() { OnClickEditMenu_DuplicateEntity(); });
	}

	void LevelEditorTab::CreateMenuTransformation(Widgets::MenuBar* pMenuBar)
	{
		Widgets::Menu* pTransformMenu = pMenuBar->AddMenu("Transformation");

		Widgets::MenuItem* pSelectionItem = pTransformMenu->AddMenuItem("Selection");
		pSelectionItem->SetShortcut("Q");
		pSelectionItem->OnClick([this]() { OnClick_SetGizmoModeSelection(); });

		Widgets::MenuItem* pTranslateItem = pTransformMenu->AddMenuItem("Translate");
		pTranslateItem->SetShortcut("W");
		pTranslateItem->OnClick([this]() { OnClick_SetGizmoModeTranslate(); });

		Widgets::MenuItem* pRotateItem = pTransformMenu->AddMenuItem("Rotate");
		pRotateItem->SetShortcut("E");
		pRotateItem->OnClick([this]() { OnClick_SetGizmoModeRotation(); });

		Widgets::MenuItem* pScaleItem = pTransformMenu->AddMenuItem("Scale");
		pScaleItem->SetShortcut("R");
		pScaleItem->OnClick([this]() { m_pViewport->GetGizmoWidget()->SetManipulatorMode(GizmoWidget::kScale); });

		pTransformMenu->AddSeparator();

		m_pSnapItem = pTransformMenu->AddMenuItem("Snap");
		m_pSnapItem->OnClick([this]() { OnClickTransformationMenu_Snap(); });
	}

	void LevelEditorTab::CreateMenuWindows(Widgets::MenuBar* pMenuBar)
	{
		Widgets::Menu* pMenu = pMenuBar->AddMenu("Windows");

		Widgets::MenuItem* pSceneTreeItem = pMenu->AddMenuItem("Scene Tree");
		pSceneTreeItem->OnClick([this]() { CreateSceneTreeViewer(m_pLeftSplit); });

		Widgets::MenuItem* pEntityItem = pMenu->AddMenuItem("Entity Properties");
		pEntityItem->OnClick([this]() { CreateEntityPropertyGrid(m_pSplit); });
	}

	void LevelEditorTab::CreateEntityPropertyGrid(Widgets::SplitVertical* pSplit)
	{
		Widgets::Frame* pFrame = new Widgets::Frame("Entity Properties");
		pFrame->OnClose([this]() { m_pEntityWidget = nullptr; delete m_pEntityModel; m_pEntityModel = nullptr; });
		pSplit->AddRightPanel(pFrame);

		Widgets::Layout* pLayout = new Widgets::Layout(0, 0, 0, 0);
		pLayout->SetSizeStyle(Widgets::Widget::SIZE_STYLE::STRETCH);
		pLayout->SetDirection(Widgets::Layout::Vertical);

		pFrame->AddWidget(pLayout);

		m_pEntityNameLabel = new Widgets::Label();
		m_pEntityNameLabel->SetSize(Core::UInt2(0, 20));
		pLayout->AddWidget(m_pEntityNameLabel);

		m_pEntityModel = new EntityModel(nullptr);
		m_pEntityWidget = new EntityWidget();
		m_pEntityWidget->SetModel(m_pEntityModel);
		pLayout->AddWidget(m_pEntityWidget);

		LevelEditorModule& levelEditorModule = LevelEditorModule::Get();
		SelectionMgr* pSelectionMgr = levelEditorModule.GetSelectionMgr();
		m_cidOnSelectionCleared_EntityProperties = pSelectionMgr->OnClear([this]() { OnSelectionCleared_EntityProperties(); });
		pSelectionMgr->OnItemAdded([this](const Os::Guid& nodeGuid) { OnAddedToSelection_EntityProperties(nodeGuid); });
		pSelectionMgr->OnItemRemoved([this](const Os::Guid& nodeGuid) { OnRemovedFromSelection_EntityProperties(nodeGuid); });

		levelEditorModule.OnRenameEntity([this](const Os::Guid& nodeGuid) { OnRenameEntity_EntityProperties(nodeGuid); });
	}

	void LevelEditorTab::CreateSceneTreeViewer(Widgets::SplitVertical* pSplit)
	{
		if (m_pSceneTreeFrame != nullptr)
			return;

		m_pSceneTreeFrame = new Widgets::Frame("Scene Tree");
		m_pSceneTreeFrame->OnClose([this]() { m_pSceneTreeFrame = nullptr; });

		pSplit->AddLeftPanel(m_pSceneTreeFrame);

		Widgets::Layout* pLayout = new Widgets::Layout();
		pLayout->SetSizeStyle(Widgets::Widget::STRETCH);
		pLayout->SetDirection(Widgets::Layout::Vertical);
		m_pSceneTreeFrame->AddWidget(pLayout);

		Editors::LevelEditorModule& levelEditorModule = Editors::LevelEditorModule::Get();
		m_pLevelTreeModel = new LevelTreeModel(levelEditorModule.GetLevelMgr()->GetSceneTree()->GetRoot());
		m_pTreeWidget = new TreeWidget();
		m_pTreeWidget->SetModel(m_pLevelTreeModel);

		pLayout->AddWidget(m_pTreeWidget);

		m_pTreeWidget->OnItemClicked(std::bind(&LevelEditorTab::OnClick_TreeItem, this, std::placeholders::_1, std::placeholders::_2));

		//callbacks
		levelEditorModule.OnAddEntity([this](const Os::Guid& nodeGuid) { OnAddEntity_SceneTree(nodeGuid); });
		levelEditorModule.OnDeleteEntity([this](const Os::Guid& nodeGuid) { OnDeleteEntity_SceneTree(nodeGuid); });
		levelEditorModule.OnRenameEntity([this](const Os::Guid& nodeGuid) { OnRenameEntity_SceneTree(nodeGuid); });
		levelEditorModule.OnDuplicateEntity([this](const Os::Guid& src, const Os::Guid& copy) { OnDuplicateEntity_SceneTree(src, copy); });
		levelEditorModule.OnNewLevel([this]() { OnNewLevel_SceneTree(); });
		levelEditorModule.OnLoadLevel([this]() { OnLoadLevel_SceneTree(); });
	}

	void LevelEditorTab::CreateRenameModalWindow(const std::function<void(const std::string& newName)>& callback) const
	{
		//show a modal window to enter the entity name
		Widgets::ModalWindow* pWindow = new Widgets::ModalWindow("Entity Name");
		pWindow->SetSize(Core::UInt2(500, 70));
		pWindow->SetSizeStyle(Widgets::Widget::DEFAULT);
		pWindow->SetPositionStyle(Widgets::Widget::HPOSITION_STYLE::CENTER, Widgets::Widget::VPOSITION_STYLE::MIDDLE);

		//vlayout
		Widgets::Layout* pVLayout = new Widgets::Layout();
		pVLayout->SetDirection(Widgets::Layout::Vertical);
		pVLayout->SetSizeStyle(Widgets::Widget::STRETCH);
		pWindow->AddWidget(pVLayout);

		//text box for the name of the entity
		Widgets::TextBox* pNameTextBox = new Widgets::TextBox();
		pNameTextBox->SetSize(Core::UInt2(0, 20));
		pNameTextBox->SetSizeStyle(Widgets::Widget::HSIZE_STRETCH | Widgets::Widget::VSIZE_DEFAULT);

		pNameTextBox->OnValidate([this, callback](const std::string& value) -> bool
			{
				callback(value);
				Widgets::WidgetMgr::Get().CloseModalWindow();
				return true;
			});
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
		pOkButton->OnClick([this, pNameTextBox, callback]() -> bool
			{
				const std::string& text = pNameTextBox->GetText();
				callback(text);
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
		pCancelButton->OnClick([]() -> bool { Widgets::WidgetMgr::Get().CloseModalWindow(); return true; });
		pHLayout->AddWidget(pCancelButton);

		Widgets::WidgetMgr::Get().OpenModalWindow(pWindow);
		Widgets::WidgetMgr::Get().SetFocus(pNameTextBox);
	}

	bool LevelEditorTab::OnClick_TreeItem(BaseModel* pModel, int rowId)
	{
		if (!m_pEntityWidget)
			return true;

		//get the entity
		LevelTreeModel* pLevelTreeModel = static_cast<LevelTreeModel*>(pModel);
		Node* pNode = pLevelTreeModel->GetSource();

		if (!pNode)
			return false;

		LevelEditorModule& levelEditorModule = LevelEditorModule::Get();
		levelEditorModule.ClearSelection();
		levelEditorModule.AddToSelection(pNode->GetConstGuid());

		return true;
	}

	void LevelEditorTab::OnClick_SetGizmoModeSelection()
	{
		//if right click, don't apply the mode
		Inputs::InputMgr& inputs = Inputs::InputMgr::Get();
		if (inputs.IsMouseRightButtonDown())
			return;

		GizmoWidget* pGizmo = m_pViewport->GetGizmoWidget();
		pGizmo->SetEnable(!pGizmo->IsEnabled());
	}

	void LevelEditorTab::OnClick_SetGizmoModeTranslate()
	{
		//if right click, don't apply the mode
		Inputs::InputMgr& inputs = Inputs::InputMgr::Get();
		if (inputs.IsMouseRightButtonDown())
			return;

		m_pViewport->GetGizmoWidget()->SetManipulatorMode(GizmoWidget::kTranslation);
	}

	void LevelEditorTab::OnClick_SetGizmoModeRotation()
	{
		//if right click, don't apply the mode
		Inputs::InputMgr& inputs = Inputs::InputMgr::Get();
		if (inputs.IsMouseRightButtonDown())
			return;

		m_pViewport->GetGizmoWidget()->SetManipulatorMode(GizmoWidget::kRotation);
	}

	void LevelEditorTab::OnSelectionCleared_EntityProperties()
	{
		if (!m_pEntityWidget)
			return;

		m_pEntityModel = new EntityModel(nullptr);
		m_pEntityWidget->SetModel(m_pEntityModel);
	}

	void LevelEditorTab::OnAddedToSelection_EntityProperties(const Os::Guid& nodeGuid)
	{
		LevelEditorModule& levelEditorModule = LevelEditorModule::Get();

		SceneTree* pSceneTree = levelEditorModule.GetLevelMgr()->GetSceneTree();
		Node* pNode = pSceneTree->GetNode(nodeGuid);
		if (!pNode)
			return;

		Entity* pEntity = pNode->ToEntity();
		if (!pEntity)
			return;

		m_pEntityModel = new EntityModel(pEntity);
		m_pEntityWidget->SetModel(m_pEntityModel);
		Widgets::WidgetMgr::Get().RequestResize();

		m_pEntityNameLabel->SetText(pEntity->GetName());
	}

	void LevelEditorTab::OnRemovedFromSelection_EntityProperties(const Os::Guid& nodeGuid)
	{
		LevelEditorModule& levelEditorModule = LevelEditorModule::Get();
		const SelectionMgr* pSelectionMgr = levelEditorModule.GetConstSelectionMgr();

		const std::list<Os::Guid>& selectionList = pSelectionMgr->GetSelectionList();
		if (selectionList.empty())
		{
			m_pEntityModel = new EntityModel(nullptr);
			m_pEntityWidget->SetModel(m_pEntityModel);
			m_pEntityNameLabel->SetText("");
			
		}
		else
		{
			const Os::Guid& lastSelectedNodeGuid = selectionList.back();
			SceneTree* pSceneTree = levelEditorModule.GetLevelMgr()->GetSceneTree();
			Node* pNode = pSceneTree->GetNode(lastSelectedNodeGuid);
			if (!pNode)
				return;

			Entity* pEntity = pNode->ToEntity();
			if (!pEntity)
				return;

			m_pEntityModel = new EntityModel(pEntity);
			m_pEntityWidget->SetModel(m_pEntityModel);
		}

		Widgets::WidgetMgr::Get().RequestResize();
	}

	void LevelEditorTab::OnRenameEntity_EntityProperties(const Os::Guid& nodeGuid)
	{
		LevelEditorModule& levelEditorModule = LevelEditorModule::Get();

		SceneTree* pSceneTree = levelEditorModule.GetLevelMgr()->GetSceneTree();
		Node* pNode = pSceneTree->GetNode(nodeGuid);
		if (!pNode)
			return;

		Entity* pEntity = pNode->ToEntity();
		if (!pEntity)
			return;

		m_pEntityNameLabel->SetText(pEntity->GetName());
	}

	void LevelEditorTab::OnSelectionCleared_Gizmo()
	{
		GizmoModel* pGizmoModel = m_pViewport->GetGizmoModel();
		pGizmoModel->SetNode(Os::Guid());
	}

	void LevelEditorTab::OnAddedToSelection_Gizmo(const Os::Guid& nodeGuid)
	{
		GizmoModel* pGizmoModel = m_pViewport->GetGizmoModel();
		pGizmoModel->SetNode(nodeGuid);
	}

	void LevelEditorTab::OnRemovedFromSelection_Gizmo(const Os::Guid& nodeGuid)
	{
		LevelEditorModule& levelEditorModule = LevelEditorModule::Get();
		const SelectionMgr* pSelectionMgr = levelEditorModule.GetConstSelectionMgr();

		const std::list<Os::Guid>& selectionList = pSelectionMgr->GetSelectionList();
		if (selectionList.empty())
		{
			GizmoModel* pGizmoModel = m_pViewport->GetGizmoModel();
			pGizmoModel->SetNode(Os::Guid());

		}
		else
		{
			GizmoModel* pGizmoModel = m_pViewport->GetGizmoModel();
			pGizmoModel->SetNode(nodeGuid);
		}
	}

	void LevelEditorTab::OnAddEntity_SceneTree(const Os::Guid& nodeGuid)
	{
		delete m_pLevelTreeModel;
		Editors::LevelEditorModule& levelEditorModule = Editors::LevelEditorModule::Get();
		m_pLevelTreeModel = new LevelTreeModel(levelEditorModule.GetLevelMgr()->GetSceneTree()->GetRoot());
		m_pTreeWidget->SetModel(m_pLevelTreeModel);
	}

	void LevelEditorTab::OnDeleteEntity_SceneTree(const Os::Guid& nodeGuid)
	{
		delete m_pLevelTreeModel;
		Editors::LevelEditorModule& levelEditorModule = Editors::LevelEditorModule::Get();
		m_pLevelTreeModel = new LevelTreeModel(levelEditorModule.GetLevelMgr()->GetSceneTree()->GetRoot());
		m_pTreeWidget->SetModel(m_pLevelTreeModel);
	}

	void LevelEditorTab::OnRenameEntity_SceneTree(const Os::Guid& nodeGuid)
	{
		delete m_pLevelTreeModel;
		m_pLevelTreeModel = new LevelTreeModel(LevelEditorModule::Get().GetLevelMgr()->GetSceneTree()->GetRoot());
		m_pTreeWidget->SetModel(m_pLevelTreeModel);
	}

	void LevelEditorTab::OnDuplicateEntity_SceneTree(const Os::Guid& src, const Os::Guid& copy)
	{
		delete m_pLevelTreeModel;
		m_pLevelTreeModel = new LevelTreeModel(LevelEditorModule::Get().GetLevelMgr()->GetSceneTree()->GetRoot());
		m_pTreeWidget->SetModel(m_pLevelTreeModel);
	}

	void LevelEditorTab::OnNewLevel_SceneTree()
	{
		delete m_pLevelTreeModel;
		m_pLevelTreeModel = new LevelTreeModel(LevelEditorModule::Get().GetLevelMgr()->GetSceneTree()->GetRoot());
		m_pTreeWidget->SetModel(m_pLevelTreeModel);
	}

	void LevelEditorTab::OnLoadLevel_SceneTree()
	{
		delete m_pLevelTreeModel;
		m_pLevelTreeModel = new LevelTreeModel(LevelEditorModule::Get().GetLevelMgr()->GetSceneTree()->GetRoot());
		m_pTreeWidget->SetModel(m_pLevelTreeModel);
	}

	void LevelEditorTab::OnClickFileMenu_LoadLevel()
	{
		//modal windows are automatically deleted when closed,sono need to delete the dialog.
		Editors::AssetDialog* pNewAssetDialog = new Editors::AssetDialog(false, Systems::kLevel);

		pNewAssetDialog->OnAssetSelected([](Systems::AssetId id) { LevelEditorModule::Get().LoadLevel(id); });
		pNewAssetDialog->Open();
	}

	void LevelEditorTab::OnClickFileMenu_Save()
	{
		LevelEditorModule& levelEditorModule = LevelEditorModule::Get();
		Systems::AssetId id = levelEditorModule.GetCurrentLoadedLevelAssetId();
		if (id.IsValid())
		{
			levelEditorModule.SaveLevel();
		}
		else
		{
			OnClickFileMenu_SaveAs();
		}
	}

	void LevelEditorTab::OnClickFileMenu_SaveAs()
	{
		AssetDialog* pDialog = new AssetDialog(true, Systems::kLevel);
		pDialog->OnAssetSelected([](Systems::AssetId id) { LevelEditorModule::Get().SaveAsLevel(id); });
		pDialog->Open();
	}

	void LevelEditorTab::OnClickEditMenu_AddEntity()
	{
		Editors::LevelEditorModule& levelEditorModule = Editors::LevelEditorModule::Get();

		Os::Guid newGuid;
		levelEditorModule.AddNewEntity(newGuid);

		if (!newGuid.IsValid())
			return;

		levelEditorModule.ClearSelection();
		levelEditorModule.AddToSelection(newGuid);
	}

	void LevelEditorTab::OnClickEditMenu_DeleteEntity()
	{
		LevelEditorModule& levelEditorModule = LevelEditorModule::Get();
		const SelectionMgr* pSelectionMgr = levelEditorModule.GetConstSelectionMgr();

		//make a copy of the selection list because the deletion operation will change the list
		const std::list<Os::Guid> selectionList = pSelectionMgr->GetSelectionList();

		for (const Os::Guid& selectedGuid : selectionList)
			levelEditorModule.DeleteEntity(selectedGuid);
	}

	void LevelEditorTab::OnClickEditMenu_RenameEntity()
	{
		LevelEditorModule& levelEditorModule = LevelEditorModule::Get();

		const SelectionMgr* pSelectionMgr = levelEditorModule.GetConstSelectionMgr();
		const std::list<Os::Guid>& selectionList = pSelectionMgr->GetSelectionList();
		if (selectionList.empty())
			return;

		const Os::Guid& lastSelectedNode = selectionList.back();

		CreateRenameModalWindow([lastSelectedNode](const std::string& newName) {
			Editors::LevelEditorModule& levelEditorModule = Editors::LevelEditorModule::Get();
			levelEditorModule.RenameEntity(lastSelectedNode, newName);
			});
	}

	void LevelEditorTab::OnClickEditMenu_DuplicateEntity()
	{
		LevelEditorModule& levelEditorModule = LevelEditorModule::Get();

		const SelectionMgr* pSelectionMgr = levelEditorModule.GetConstSelectionMgr();
		const std::list<Os::Guid>& selectionList = pSelectionMgr->GetSelectionList();
		if (selectionList.empty())
			return;

		std::vector<Os::Guid> newNodes;
		for (const Os::Guid& selectedGuid : selectionList)
		{
			Os::Guid copy;
			levelEditorModule.DuplicateEntity(selectedGuid, copy);
			newNodes.push_back(copy);
		}

		levelEditorModule.ClearSelection();
		for (const Os::Guid& guid : newNodes)
			levelEditorModule.AddToSelection(guid);
	}

	void LevelEditorTab::OnClickTransformationMenu_Snap()
	{
		GizmoWidget* pGizmo = m_pViewport->GetGizmoWidget();
		bool enabled = pGizmo->SnappingEnabled();
		pGizmo->SetSnapping(!enabled);
		m_pSnapItem->SetChecked(!enabled);
	}
}
