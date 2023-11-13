/********************************************************************/
/* © 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/LevelEditor/LevelEditorTab.h"

#include "Editors/LevelEditor/LevelEditor.h"
#include "Editors/LevelEditor/LevelEditorViewportWidget.h"
#include "Editors/LevelEditor/SceneTree/Entity.h"
#include "Editors/LevelEditor/SceneTree/Node.h"
#include "Editors/LevelEditor/SceneTree/SceneTree.h"

#include "Editors/Widgets/Entity/EntityModel.h"
#include "Editors/Widgets/Entity/EntityWidget.h"
#include "Editors/Widgets/Tree/LevelTreeModel.h"
#include "Editors/Widgets/Tree/TreeWidget.h"

#include "Rendering/RenderModule.h"

#include "Widgets/Container.h"
#include "Widgets/Button.h"
#include "Widgets/Label.h"
#include "Widgets/Layout.h"
#include "Widgets/ModalWindow.h"
#include "Widgets/SplitVertical.h"
#include "Widgets/TabContainer.h"
#include "Widgets/TextBox.h"
#include "Widgets/Viewport.h"
#include "Widgets/WidgetMgr.h"

namespace Editors
{
	LevelEditorTab::LevelEditorTab(Widgets::Widget* pParent)
		: Widgets::Tab()
		, m_enableViewportControl(false)
		, m_pEntityNameLabel(nullptr)
		, m_pEntityModel(nullptr)
		, m_pEntityWidget(nullptr)
		, m_pLevelTreeModel(nullptr)
	{
		//create the render target
		int width = 1280;
		int height = 720;
		m_aspectRatio = width / static_cast<float>(height);
		m_pRenderTarget = Rendering::RenderModule::Get().CreateRenderTarget(width, height);

		//create the split between viewport and right panel
		Widgets::SplitVertical* pSplit = new Widgets::SplitVertical();
		pSplit->SetSizeStyle(Widgets::Widget::STRETCH);
		pSplit->SetRightPanelWidth(400);
		pSplit->SetResizePolicy(Widgets::SplitVertical::KeepRightSize);
		AddWidget(pSplit);

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

		CreateEntityPropertyGrid(pSplit);
		CreateSceneTreeViewer(pLeftSplit);
	}

	LevelEditorTab::~LevelEditorTab()
	{}

	void LevelEditorTab::CreateEntityPropertyGrid(Widgets::SplitVertical* pSplit)
	{
		Widgets::Layout* pLayout = new Widgets::Layout(0, 0, 0, 0);
		pLayout->SetSizeStyle(Widgets::Widget::SIZE_STYLE::STRETCH);
		pLayout->SetDirection(Widgets::Layout::Vertical);

		pSplit->AddRightPanel(pLayout);

		m_pEntityNameLabel = new Widgets::Label();
		m_pEntityNameLabel->SetSize(DirectX::XMUINT2(0, 20));
		pLayout->AddWidget(m_pEntityNameLabel);

		m_pEntityModel = new EntityModel(nullptr);
		m_pEntityWidget = new EntityWidget();
		m_pEntityWidget->SetModel(m_pEntityModel);
		pLayout->AddWidget(m_pEntityWidget);
	}

	void LevelEditorTab::CreateSceneTreeViewer(Widgets::SplitVertical* pSplit)
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
			pButton->OnClick(std::bind(&LevelEditorTab::OnClick_AddEntity, this));
		}

		//remove entity
		{
			Widgets::Button* pButton = new Widgets::Button(BUTTON_SIZE, BUTTON_SIZE, 0, 0);
			pMenuLayout->AddWidget(pButton);
			Widgets::Label* pButtonLabel = new Widgets::Label(0, 0, 1, "-");
			pButtonLabel->SetX(7);
			pButton->AddWidget(pButtonLabel);
			pButton->OnClick(std::bind(&LevelEditorTab::OnClick_RemoveEntity, this));
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

		Editors::LevelEditor& levelEditorModule = Editors::LevelEditor::Get();
		m_pLevelTreeModel = new LevelTreeModel(levelEditorModule.GetLevel().GetSceneTree()->GetRoot());
		m_pTreeWidget = new TreeWidget();
		m_pTreeWidget->SetModel(m_pLevelTreeModel);

		pLayout->AddWidget(m_pTreeWidget);

		m_pTreeWidget->OnItemClicked(std::bind(&LevelEditorTab::OnClick_TreeItem, this, std::placeholders::_1, std::placeholders::_2));
	}

	bool LevelEditorTab::OnClick_AddEntity()
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
				Editors::LevelEditor& levelEditorModule = Editors::LevelEditor::Get();

				const std::string& text = pNameTextBox->GetText();
				levelEditorModule.AddNewEntity(text);

				delete m_pLevelTreeModel;
				m_pLevelTreeModel = new LevelTreeModel(levelEditorModule.GetLevel().GetSceneTree()->GetRoot());
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

	bool LevelEditorTab::OnClick_RemoveEntity()
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
				Editors::LevelEditor& levelEditorModule = Editors::LevelEditor::Get();

				levelEditorModule.DeleteEntity(pNode);
				delete m_pLevelTreeModel;
				m_pLevelTreeModel = new LevelTreeModel(levelEditorModule.GetLevel().GetSceneTree()->GetRoot());
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

	bool LevelEditorTab::OnClick_TreeItem(BaseModel* pModel, int rowId)
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
}
