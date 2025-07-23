/********************************************************************/
/* � 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/LevelEditor/Widgets/LevelEditor.h"

#include "Core/Log/LogModule.h"

#include "Editors/LevelEditor/LevelEditorModule.h"
#include "Editors/LevelEditor/LevelListModel.h"
#include "Editors/LevelEditor/LevelMgr.h"
#include "Editors/LevelEditor/SceneTreeModel.h"
#include "Editors/LevelEditor/SceneTree/Entity.h"
#include "Editors/LevelEditor/SceneTree/Node.h"
#include "Editors/LevelEditor/SceneTree/SceneTree.h"
#include "Editors/LevelEditor/SelectionMgr.h"
#include "Editors/LevelEditor/Widgets/LevelEditorViewportWidget.h"
#include "Editors/LevelEditor/Widgets/GizmoModel.h"
#include "Editors/LevelEditor/Widgets/GizmoWidget.h"

#include "Editors/Widgets/Dialog/AssetDialog.h"
#include "Editors/Widgets/Dialog/OkCancelDialog.h"
#include "Editors/Widgets/Dialog/UserInputDialog.h"
#include "Editors/Widgets/Entity/EntityModel.h"
#include "Editors/Widgets/Entity/EntityWidget.h"

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
#include "Widgets/Models/SelectionModel.h"
#include "Widgets/Models/SelectionRow.h"
#include "Widgets/SplitVertical.h"
#include "Widgets/Tab.h"
#include "Widgets/TabContainer.h"
#include "Widgets/TextBox.h"
#include "Widgets/Viewport.h"
#include "Widgets/WidgetMgr.h"
#include "Widgets/Widgets/Frame.h"
#include "Widgets/Widgets/TableView.h"
#include "Widgets/Widgets/TreeView.h"

namespace Editors
{
	LevelEditor::LevelEditor()
		: Core::Singleton<LevelEditor>()
		, m_enableViewportControl(false)
		, m_pEntityNameLabel(nullptr)
		, m_pEntityModel(nullptr)
		, m_pEntityWidget(nullptr)
		, m_pSceneTreeFrame(nullptr)
		, m_pLeftSplit(nullptr)
		, m_cidOnSelectionCleared_EntityProperties()
		, m_selectedLevelInLevelList()
		, m_pLeftTabContainer(nullptr)
		, m_pLevelListModel(nullptr)
		, m_pSnapItem(nullptr)
		, m_pSplit(nullptr)
		, m_pViewport(nullptr)
		, m_pSceneTree(nullptr)
		, m_pSceneTreeModel(nullptr)
	{ }

	LevelEditor::~LevelEditor()
	{ }

	void LevelEditor::CreateEditor(Widgets::Widget* pParent)
	{
		Widgets::Tab* pViewportTab = new Widgets::Tab();
		Widgets::TabContainer* pTabContainer = dynamic_cast<Widgets::TabContainer*>(pParent);
		if (pTabContainer)
		{
			pTabContainer->AddTab("Level", pViewportTab);
		}
		else
		{
			pParent->AddWidget(pViewportTab);
		}

		//create the render target
		int width = 1280;
		int height = 720;

		Widgets::Layout* pInternalLayout = new Widgets::Layout();
		pInternalLayout->SetDirection(Widgets::Layout::Direction::Vertical);
		pInternalLayout->SetSizeStyle(Widgets::Widget::SIZE_STYLE::STRETCH);
		pViewportTab->AddWidget(pInternalLayout);

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
		m_pLeftSplit->SetLeftPanelWidth(300);
		m_pLeftSplit->SetResizePolicy(Widgets::SplitVertical::KeepLeftSize);
		m_pLeftSplit->AddRightPanel(m_pViewport);


		m_pSplit->AddLeftPanel(m_pLeftSplit);

		CreateEntityPropertyGrid(m_pSplit);

		m_pLeftTabContainer = new Widgets::TabContainer(false);
		m_pLeftSplit->AddLeftPanel(m_pLeftTabContainer);

		CreateSceneTreeViewer(m_pLeftTabContainer);
		CreateLevelBrowser(m_pLeftTabContainer);
		//m_pLeftTabContainer->SetSelectedTab(0);

		//gizmo callback registration
		LevelEditorModule& levelEditorModule = LevelEditorModule::Get();
		SelectionMgr* pSelectionMgr = levelEditorModule.GetSelectionMgr();
		pSelectionMgr->OnClear([this]() { OnSelectionCleared_Gizmo(); });
		pSelectionMgr->OnItemAdded([this](const Core::Guid& nodeGuid) { OnAddedToSelection_Gizmo(nodeGuid); });
		pSelectionMgr->OnItemRemoved([this](const Core::Guid& nodeGuid) { OnRemovedFromSelection_Gizmo(nodeGuid); });

		levelEditorModule.OnNewLevel([this](const Systems::AssetMetadata& metadata) {  m_pLevelListModel->AddNewLevel(metadata); });
		levelEditorModule.OnBeforeDeleteLevel([this](const Systems::AssetMetadata& metadata) { OnLevelEditorModule_BeforeDeleteLevel(metadata); });
		levelEditorModule.OnRenameLevel([this](Systems::NewAssetId id, const std::string& newName) { OnLevelEditorModule_RenameLevel(id, newName); });
		levelEditorModule.OnOpenLevel([this]() { OnLevelEditorModule_OpenLevel(); });
		levelEditorModule.OnAddGameObject([this](const Systems::GameObject* pGo, const Systems::GameObject* pGoParent) { OnLevelEditorModule_AddGameObject(pGo, pGoParent); });
		levelEditorModule.OnRenameGameObject([this](const Core::Guid& guid, const std::string& newName) { OnLevelEditorModule_RenameGameObject(guid, newName); });
		levelEditorModule.OnBeforeDeleteGameObject([this](const Core::Guid& guid) { OnLevelEditorModule_DeleteGameObject(guid); });
		levelEditorModule.OnReparentGameObject([this](const Systems::GameObject* pGo, const Systems::GameObject* pGoOldParent, const Systems::GameObject* pGoNewParent) { OnLevelEditorModule_ReparentGameObject(pGo, pGoOldParent, pGoNewParent); });
	}

	void LevelEditor::CreateMenuFile(Widgets::MenuBar* pMenuBar)
	{
		Widgets::Menu* pEditMenu = pMenuBar->AddMenu("File");

		Widgets::MenuItem* pNewItem = pEditMenu->AddMenuItem("New Level...");
		pNewItem->SetShortcut("Ctrl+N");
		pNewItem->OnClick([this]() { OnClickFileMenu_NewLevel(); });

		Widgets::MenuItem* pOpenItem = pEditMenu->AddMenuItem("Open");
		pOpenItem->SetShortcut("Ctrl+O");
		pOpenItem->OnClick([this]() { OnClickFileMenu_OpenLevel(); });

		Widgets::MenuItem* pSaveItem = pEditMenu->AddMenuItem("Save");
		pSaveItem->SetShortcut("Ctrl+S");
		pSaveItem->OnClick([this]() { OnClickFileMenu_SaveLevel(); });

		Widgets::MenuItem* pRenameItem = pEditMenu->AddMenuItem("Rename...");
		pRenameItem->SetShortcut("F2");
		pRenameItem->OnClick([this]() { OnClickFileMenu_RenameLevel(); });

		Widgets::MenuItem* pDeleteItem = pEditMenu->AddMenuItem("Delete Level...");
		pDeleteItem->SetShortcut("Del");
		pDeleteItem->OnClick([this]() { OnClickFileMenu_DeleteLevel(); });

		Editors::LevelEditorModule& levelEditorModule = Editors::LevelEditorModule::Get();
		levelEditorModule.OnNewLevel([pSaveItem](const Systems::AssetMetadata& metadata) { pSaveItem->SetText("Save"); });
		levelEditorModule.OnOpenLevel([pSaveItem]() { pSaveItem->SetText("Save " + Editors::LevelEditorModule::Get().GetCurrentLoadedLevelName()); });
		levelEditorModule.OnSaveLevel([pSaveItem]() { pSaveItem->SetText("Save " + Editors::LevelEditorModule::Get().GetCurrentLoadedLevelName()); });
	}

	void LevelEditor::CreateMenuEdit(Widgets::MenuBar* pMenuBar)
	{
		Widgets::Menu* pEditMenu = pMenuBar->AddMenu("Edit");

		Widgets::MenuItem* pAddItem = pEditMenu->AddMenuItem("Add Game Object");
		pAddItem->OnClick([this]() { OnClickEditMenu_AddGameObject(); });

		Widgets::MenuItem* pDeleteItem = pEditMenu->AddMenuItem("Delete Game Object");
		pDeleteItem->SetShortcut("Del");
		pDeleteItem->OnClick([this]() { OnClickEditMenu_DeleteGameObject(); });

		Widgets::MenuItem* pRenameItem = pEditMenu->AddMenuItem("Rename Game Object...");
		pRenameItem->OnClick([this]() { OnClickEditMenu_RenameGameObject(); });

		Widgets::MenuItem* pDuplicateItem = pEditMenu->AddMenuItem("Duplicate");
		pDuplicateItem->SetShortcut("Ctrl+D");
		pDuplicateItem->OnClick([this]() { OnClickEditMenu_DuplicateEntity(); });

		Widgets::MenuItem* pReparentItem = pEditMenu->AddMenuItem("Reparent Game Object...");
		pReparentItem->OnClick([this]() { OnClickEditMenu_ReparentGameObject(); });
	}

	void LevelEditor::CreateMenuTransformation(Widgets::MenuBar* pMenuBar)
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

	void LevelEditor::CreateMenuWindows(Widgets::MenuBar* pMenuBar)
	{
		Widgets::Menu* pMenu = pMenuBar->AddMenu("Windows");

		Widgets::MenuItem* pSceneTreeItem = pMenu->AddMenuItem("Scene Tree");
		pSceneTreeItem->OnClick([this]() { CreateSceneTreeViewer(m_pLeftTabContainer); });

		Widgets::MenuItem* pEntityItem = pMenu->AddMenuItem("Entity Properties");
		pEntityItem->OnClick([this]() { CreateEntityPropertyGrid(m_pSplit); });
	}

	void LevelEditor::CreateEntityPropertyGrid(Widgets::SplitVertical* pSplit)
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
		//pSelectionMgr->OnItemAdded([this](const Core::Guid& nodeGuid) { OnAddedToSelection_EntityProperties(nodeGuid); });
		//pSelectionMgr->OnItemRemoved([this](const Core::Guid& nodeGuid) { OnRemovedFromSelection_EntityProperties(nodeGuid); });
	}

	void LevelEditor::CreateSceneTreeViewer(Widgets::TabContainer* pParent)
	{
		if (m_pSceneTreeFrame != nullptr)
			return;

		const std::string title = "Scene Tree";
		m_pSceneTreeFrame = new Widgets::Frame(title);
		m_pSceneTreeFrame->OnClose([this, pParent]() 
			{ 
				pParent->CloseTab(m_pSceneTreeFrame); m_pSceneTreeFrame = nullptr; 
			});

		pParent->AddTab(title, m_pSceneTreeFrame);

		m_pSceneTree = new Widgets::TreeView();
		m_pSceneTree->SetMultiSelection(true);
		m_pSceneTreeFrame->AddWidget(m_pSceneTree);
	}

	void LevelEditor::CreateLevelBrowser(Widgets::TabContainer* pParent)
	{
		Widgets::Frame* pLevelBrowser = new Widgets::Frame("Level Browser");
		pParent->AddTab("Level Browser", pLevelBrowser);

		Widgets::TableView* pLevelTableView = new Widgets::TableView();
		pLevelBrowser->AddWidget(pLevelTableView);

		m_pLevelListModel = new LevelListModel();
		pLevelTableView->SetModel(m_pLevelListModel);

		Widgets::SelectionModel* pSelectionModel = m_pLevelListModel->GetSelectionModel();
		pSelectionModel->OnSelectionChanged([this](const std::vector<Widgets::SelectionRow>& selected, const std::vector<Widgets::SelectionRow>& deselected) 
			{
				if (!selected.empty())
				{
					Systems::NewAssetId id = m_pLevelListModel->GetAssetId(selected[0].GetStartIndex());
					if (id.IsValid())
					{
						m_selectedLevelInLevelList = id;
						return;
					}
				}

				if (!deselected.empty())
				{
					m_selectedLevelInLevelList = Systems::NewAssetId::INVALID;
				}
			});
	}

	void LevelEditor::CreateRenameModalWindow(const std::function<void(const std::string& newName)>& callback) const
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

	void LevelEditor::OnClick_SetGizmoModeSelection()
	{
		//if right click, don't apply the mode
		Inputs::InputMgr& inputs = Inputs::InputMgr::Get();
		if (inputs.IsMouseRightButtonDown())
			return;

		GizmoWidget* pGizmo = m_pViewport->GetGizmoWidget();
		pGizmo->SetEnable(!pGizmo->IsEnabled());
	}

	void LevelEditor::OnClick_SetGizmoModeTranslate()
	{
		//if right click, don't apply the mode
		Inputs::InputMgr& inputs = Inputs::InputMgr::Get();
		if (inputs.IsMouseRightButtonDown())
			return;

		m_pViewport->GetGizmoWidget()->SetManipulatorMode(GizmoWidget::kTranslation);
	}

	void LevelEditor::OnClick_SetGizmoModeRotation()
	{
		//if right click, don't apply the mode
		Inputs::InputMgr& inputs = Inputs::InputMgr::Get();
		if (inputs.IsMouseRightButtonDown())
			return;

		m_pViewport->GetGizmoWidget()->SetManipulatorMode(GizmoWidget::kRotation);
	}

	void LevelEditor::OnSelectionCleared_EntityProperties()
	{
		if (!m_pEntityWidget)
			return;

		m_pEntityModel = new EntityModel(nullptr);
		m_pEntityWidget->SetModel(m_pEntityModel);
	}

	void LevelEditor::OnAddedToSelection_EntityProperties(const Core::Guid& nodeGuid)
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

	void LevelEditor::OnRemovedFromSelection_EntityProperties(const Core::Guid& nodeGuid)
	{
		LevelEditorModule& levelEditorModule = LevelEditorModule::Get();
		const SelectionMgr* pSelectionMgr = levelEditorModule.GetConstSelectionMgr();

		const std::list<Core::Guid>& selectionList = pSelectionMgr->GetSelectionList();
		if (selectionList.empty())
		{
			m_pEntityModel = new EntityModel(nullptr);
			m_pEntityWidget->SetModel(m_pEntityModel);
			m_pEntityNameLabel->SetText("");
			
		}
		else
		{
			const Core::Guid& lastSelectedNodeGuid = selectionList.back();
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

	void LevelEditor::OnRenameEntity_EntityProperties(const Core::Guid& nodeGuid)
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

	void LevelEditor::OnSelectionCleared_Gizmo()
	{
		GizmoModel* pGizmoModel = m_pViewport->GetGizmoModel();
		pGizmoModel->SetNode(Core::Guid());
	}

	void LevelEditor::OnAddedToSelection_Gizmo(const Core::Guid& nodeGuid)
	{
		GizmoModel* pGizmoModel = m_pViewport->GetGizmoModel();
		pGizmoModel->SetNode(nodeGuid);
	}

	void LevelEditor::OnRemovedFromSelection_Gizmo(const Core::Guid& nodeGuid)
	{
		LevelEditorModule& levelEditorModule = LevelEditorModule::Get();
		const SelectionMgr* pSelectionMgr = levelEditorModule.GetConstSelectionMgr();

		const std::list<Core::Guid>& selectionList = pSelectionMgr->GetSelectionList();
		if (selectionList.empty())
		{
			GizmoModel* pGizmoModel = m_pViewport->GetGizmoModel();
			pGizmoModel->SetNode(Core::Guid());

		}
		else
		{
			GizmoModel* pGizmoModel = m_pViewport->GetGizmoModel();
			pGizmoModel->SetNode(nodeGuid);
		}
	}

	void LevelEditor::OnClickFileMenu_NewLevel()
	{
		//modal windows are automatically deleted when closed,so no need to delete the dialog.
		UserInputDialog* pDialog = new UserInputDialog("New Asset Name");
		pDialog->OnInputValidated([this](const std::string& input)
			{
				Systems::LevelAsset* pLevel = LevelEditorModule::Get().CreateNewLevel(input);
			});
		pDialog->Open();
	}

	void LevelEditor::OnClickFileMenu_OpenLevel()
	{
		if (!m_selectedLevelInLevelList.IsValid())
			return;

		LevelEditorModule& levelEditorModule = LevelEditorModule::Get();
		levelEditorModule.OpenLevel(m_selectedLevelInLevelList);
	}

	void LevelEditor::OnClickFileMenu_SaveLevel()
	{
		LevelEditorModule& levelEditorModule = LevelEditorModule::Get();
		Systems::NewAssetId id = levelEditorModule.GetCurrentLoadedLevelAssetId();
		if (id.IsValid())
		{
			levelEditorModule.SaveLevel();
		}
	}

	void LevelEditor::OnClickFileMenu_DeleteLevel()
	{
		if (!m_selectedLevelInLevelList.IsValid())
			return;

		OkCancelDialog* pDialog = new OkCancelDialog("Delete", "Are you sure you want to delete this level?");
		pDialog->OnOk([this]() { LevelEditorModule::Get().DeleteLevel(m_selectedLevelInLevelList); });
		pDialog->Open();
	}

	void LevelEditor::OnClickFileMenu_RenameLevel()
	{
		if (!m_selectedLevelInLevelList.IsValid())
		{
			Core::LogModule::Get().LogError("You need to select a level to rename.");
			return;
		}

		UserInputDialog* pDialog = new UserInputDialog("New name");
		pDialog->OnInputValidated([this](const std::string& input)
			{
				LevelEditorModule::Get().RenameLevel(m_selectedLevelInLevelList, input);
			});
		pDialog->Open();
	}

	void LevelEditor::OnClickEditMenu_AddGameObject()
	{
		LevelEditorModule& levelEditorModule = Editors::LevelEditorModule::Get();

		SelectionMgr* pSelectionMgr = levelEditorModule.GetSelectionMgr();

		Core::Guid parentGuid;
		if (!pSelectionMgr->GetSelectionList().empty())
			parentGuid = pSelectionMgr->GetSelectionList().back();

		Core::Guid newGuid;
		levelEditorModule.AddGameObject(parentGuid, newGuid);

		if (!newGuid.IsValid())
			return;

		m_pSceneTreeModel->SelectGameObject(newGuid);
	}

	void LevelEditor::OnClickEditMenu_DeleteGameObject()
	{
		LevelEditorModule& levelEditorModule = LevelEditorModule::Get();
		const SelectionMgr* pSelectionMgr = levelEditorModule.GetConstSelectionMgr();

		//make a copy of the selection list because the deletion operation will change the list
		const std::list<Core::Guid> selectionList = pSelectionMgr->GetSelectionList();

		for (const Core::Guid& selectedGuid : selectionList)
			levelEditorModule.DeleteGameObject(selectedGuid);
	}

	void LevelEditor::OnClickEditMenu_RenameGameObject()
	{
		LevelEditorModule& levelEditorModule = LevelEditorModule::Get();

		const SelectionMgr* pSelectionMgr = levelEditorModule.GetConstSelectionMgr();
		const std::list<Core::Guid>& selectionList = pSelectionMgr->GetSelectionList();
		if (selectionList.empty())
			return;

		const Core::Guid& lastSelectedNode = selectionList.back();

		CreateRenameModalWindow([lastSelectedNode](const std::string& newName) 
			{
				Editors::LevelEditorModule& levelEditorModule = Editors::LevelEditorModule::Get();
				levelEditorModule.RenameGameObject(lastSelectedNode, newName);
			});
	}

	void LevelEditor::OnClickEditMenu_DuplicateEntity()
	{
		LevelEditorModule& levelEditorModule = LevelEditorModule::Get();

		const SelectionMgr* pSelectionMgr = levelEditorModule.GetConstSelectionMgr();
		const std::list<Core::Guid>& selectionList = pSelectionMgr->GetSelectionList();
		if (selectionList.empty())
			return;

		std::vector<Core::Guid> newNodes;
		for (const Core::Guid& selectedGuid : selectionList)
		{
			Core::Guid copy;
			levelEditorModule.DuplicateEntity(selectedGuid, copy);
			newNodes.push_back(copy);
		}

		levelEditorModule.ClearSelection();
		for (const Core::Guid& guid : newNodes)
			levelEditorModule.AddToSelection(guid);
	}

	void LevelEditor::OnClickEditMenu_ReparentGameObject()
	{
		LevelEditorModule& levelEditorModule = LevelEditorModule::Get();
		SelectionMgr* pSelectionMgr = levelEditorModule.GetSelectionMgr();

		//reparenting will change the selection so copy the selection list.
		const std::list<Core::Guid> selectionList = pSelectionMgr->GetSelectionList();

		if (selectionList.size() < 2)
			return;

		Core::Guid newParent = selectionList.back();
		std::list<Core::Guid>::const_iterator endIt = selectionList.cend();
		--endIt;

		for (std::list<Core::Guid>::const_iterator it = selectionList.cbegin(); it != endIt; ++it)
		{
			levelEditorModule.ReparentGameObject(newParent, *it);
		}
	}

	void LevelEditor::OnClickTransformationMenu_Snap()
	{
		GizmoWidget* pGizmo = m_pViewport->GetGizmoWidget();
		bool enabled = pGizmo->SnappingEnabled();
		pGizmo->SetSnapping(!enabled);
		m_pSnapItem->SetChecked(!enabled);
	}

	void LevelEditor::OnLevelEditorModule_BeforeDeleteLevel(const Systems::AssetMetadata& metadata)
	{
		m_pLevelListModel->RemoveLevel(metadata);
	}

	void LevelEditor::OnLevelEditorModule_RenameLevel(Systems::NewAssetId id, const std::string& newName)
	{
		m_pLevelListModel->RenameLevel(id, newName);
	}

	void LevelEditor::OnLevelEditorModule_OpenLevel()
	{
		const Systems::LevelAsset* pLevel = LevelEditorModule::Get().GetCurrentLoadedLevel();

		m_pSceneTreeModel = new SceneTreeModel(pLevel);
		m_pSceneTree->SetModel(m_pSceneTreeModel);

		Widgets::SelectionModel* pSelectionModel = m_pSceneTreeModel->GetSelectionModel();
		pSelectionModel->OnSelectionChanged([this](const std::vector<Widgets::SelectionRow>& selected, const std::vector<Widgets::SelectionRow>& deselected)
			{
				for (const Widgets::SelectionRow& row : deselected)
				{
					Core::Guid guid = m_pSceneTreeModel->FindGameObject(row.GetStartIndex());
					LevelEditorModule::Get().RemoveFromSelection(guid);
				}
				
				for (const Widgets::SelectionRow& row : selected)
				{
					Core::Guid guid = m_pSceneTreeModel->FindGameObject(row.GetStartIndex());
					LevelEditorModule::Get().AddToSelection(guid);
				}
			});
	}

	void LevelEditor::OnLevelEditorModule_AddGameObject(const Systems::GameObject* pGo, const Systems::GameObject* pGoParent)
	{
		m_pSceneTreeModel->AddGameObject(pGo, pGoParent);
	}

	void LevelEditor::OnLevelEditorModule_RenameGameObject(const Core::Guid& guid, const std::string& newName)
	{
		m_pSceneTreeModel->RenameGameObject(guid, newName);
	}

	void LevelEditor::OnLevelEditorModule_DeleteGameObject(const Core::Guid& guid)
	{
		m_pSceneTreeModel->RemoveGameObject(guid);
	}

	void LevelEditor::OnLevelEditorModule_ReparentGameObject(const Systems::GameObject* pGo, const Systems::GameObject* pGoOldParent, const Systems::GameObject* pGoNewParent)
	{
		m_pSceneTreeModel->ReparentGameObject(pGo, pGoOldParent, pGoNewParent);
	}
}
