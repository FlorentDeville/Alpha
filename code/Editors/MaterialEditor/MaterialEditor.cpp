/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/MaterialEditor/MaterialEditor.h"

#include "Editors/MaterialEditor/ShaderEditorModule.h"
#include "Editors/MaterialEditor/ShaderListModel.h"
#include "Editors/Widgets/Dialog/OkCancelDialog.h"
#include "Editors/Widgets/Dialog/UserInputDialog.h"
#include "Editors/Widgets/PropertyGrid/ItemFactory/PropertyGridItemFactory_MaterialParameterDescription.h"
#include "Editors/Widgets/PropertyGrid/PropertyGridPopulator.h"
#include "Editors/Widgets/PropertyGrid/PropertyGridWidget.h"

#include "OsWin/Process.h"

#include "Systems/Assets/AssetMgr.h"
#include "Systems/Assets/AssetObjects/AssetUtil.h"
#include "Systems/Container/Container.h"
#include "Systems/Container/ContainerMgr.h"
#include "Systems/Objects/AssetObject.h"

#include "Widgets/Button.h"
#include "Widgets/Label.h"
#include "Widgets/Layout.h"
#include "Widgets/Menu.h"
#include "Widgets/MenuBar.h"
#include "Widgets/MenuItem.h"
#include "Widgets/Models/SelectionModel.h"
#include "Widgets/Models/SelectionRow.h"
#include "Widgets/SplitHorizontal.h"
#include "Widgets/SplitVertical.h"
#include "Widgets/Tab.h"
#include "Widgets/TabContainer.h"
#include "Widgets/Text.h"
#include "Widgets/Viewport_v2.h"
#include "Widgets/Widgets/TableView.h"

#include <filesystem>

namespace Editors
{
	MaterialEditor::MaterialEditor()
		: Core::Singleton<MaterialEditor>()
		, m_pShaderListLayout(nullptr)
		, m_pLogText(nullptr)
		, m_pPropertyGrid(nullptr)
		, m_pShaderListModel(nullptr)
		, m_selectedMaterialId(Systems::NewAssetId::INVALID)
		, m_pPropertyGridPopulator(new PropertyGridPopulator())
	{}

	MaterialEditor::~MaterialEditor()
	{
		delete m_pPropertyGridPopulator;
	}

	void MaterialEditor::CreateEditor(const MaterialEditorParameter& parameter)
	{
		//create the widgets
		Widgets::Tab* pViewportTab = new Widgets::Tab();
		Widgets::TabContainer* pTabContainer = dynamic_cast<Widgets::TabContainer*>(parameter.m_pParent);
		if (pTabContainer)
		{
			pTabContainer->AddTab("Material", pViewportTab);
		}
		else
		{
			parameter.m_pParent->AddWidget(pViewportTab);
		}

		Widgets::Layout* pInternalLayout = new Widgets::Layout();
		pInternalLayout->SetDirection(Widgets::Layout::Direction::Vertical);
		pInternalLayout->SetSizeStyle(Widgets::Widget::SIZE_STYLE::STRETCH);
		pViewportTab->AddWidget(pInternalLayout);

		CreateMenu(pInternalLayout);

		//create the split
		Widgets::SplitVertical* pSplit = new Widgets::SplitVertical();
		pSplit->SetSizeStyle(Widgets::Widget::HSIZE_STRETCH | Widgets::Widget::VSIZE_STRETCH);
		pSplit->SetLeftPanelWidth(500);

		pInternalLayout->AddWidget(pSplit);

		//create a button and label per shader
		m_pShaderListLayout = new Widgets::Layout(0, 0, 0, 0);
		m_pShaderListLayout->SetSizeStyle(Widgets::Widget::STRETCH);
		m_pShaderListLayout->SetDirection(Widgets::Layout::Direction::Vertical);
		pSplit->AddLeftPanel(m_pShaderListLayout);

		CreateShadersList();

		//create the horizontal split between the viewport and the property grid
		Widgets::SplitHorizontal* pHSplit = new Widgets::SplitHorizontal();
		pHSplit->SetSizeStyle(Widgets::Widget::HSIZE_STRETCH | Widgets::Widget::VSIZE_STRETCH);
		pHSplit->SetTopPanelHeight(500);
		pSplit->AddRightPanel(pHSplit);

		//add viewport
		{
			//create left viewport
			Widgets::Viewport_v2* pViewport = new Widgets::Viewport_v2(1080, 789);
			pViewport->SetSizeStyle(Widgets::Widget::HSIZE_STRETCH | Widgets::Widget::VSIZE_STRETCH);
			//pViewport->OnFocusGained([this](const Widgets::FocusEvent&) { m_enableViewportControl = true; });
			//pViewport->OnFocusLost([this](const Widgets::FocusEvent&) { m_enableViewportControl = false; });
			pHSplit->AddTopPanel(pViewport);
		}

		//right panel : add option layout
		Widgets::Layout* pRightPanelLayout = new Widgets::Layout(0, 0, 0, 0);
		pRightPanelLayout->SetSizeStyle(Widgets::Widget::HSIZE_STRETCH | Widgets::Widget::VSIZE_STRETCH);
		pRightPanelLayout->SetDirection(Widgets::Layout::Direction::Vertical);
		pHSplit->AddBottomPanel(pRightPanelLayout);
		
		//horizontal layout for buttons
		Widgets::Layout* pButtonLayout = new Widgets::Layout(0, 20, 0, 0);
		pButtonLayout->SetSizeStyle(Widgets::Widget::HSIZE_STRETCH | Widgets::Widget::VSIZE_DEFAULT);
		pButtonLayout->SetDirection(Widgets::Layout::Direction::Horizontal);
		pRightPanelLayout->AddWidget(pButtonLayout);

		//add compile button
		{
			Widgets::Button* pButton = new Widgets::Button(60, 0, 0, 0);
			pButton->SetSizeStyle(Widgets::Widget::VSIZE_STRETCH);
			pButton->OnClick([this]() -> bool { return OnCompileClicked(); });
			pButtonLayout->AddWidget(pButton);

			Widgets::Label* pLabel = new Widgets::Label(0, 0, 1, "Compile");
			pButton->AddWidget(pLabel);
		}

		//add open button
		{
			Widgets::Button* pButton = new Widgets::Button(50, 0, 0, 0);
			pButton->SetSizeStyle(Widgets::Widget::VSIZE_STRETCH);
			//pButton->OnClick([this, ii](int x, int y) -> bool { OnMeshEntryClicked(ii); return true; });
			pButtonLayout->AddWidget(pButton);

			Widgets::Label* pLabel = new Widgets::Label(0, 0, 1, "Open...");
			pButton->AddWidget(pLabel);
		}

		{
			Widgets::Button* pButton = new Widgets::Button(50, 0, 0, 0);
			pButton->SetSizeStyle(Widgets::Widget::HSIZE_STRETCH);
			pRightPanelLayout->AddWidget(pButton);
			Widgets::Label* pLabel = new Widgets::Label(0, 0, 1, "Button");
			pButton->AddWidget(pLabel);
		}

		//add property grid
		{
			m_pPropertyGrid = new PropertyGridWidget();
			pRightPanelLayout->AddWidget(m_pPropertyGrid);

			m_pPropertyGridPopulator->Init(m_pPropertyGrid);
			m_pPropertyGridPopulator->OnDataChanged([this]() {PropertyGridPopulator_OnDataChanged(); });

			Core::Sid typenameSid = Systems::TypeResolver<Systems::MaterialParameterDescription>::GetType()->GetSid();
			m_pPropertyGridPopulator->RegisterItemFactory(typenameSid, new PropertyGridItemFactory_MaterialParameterDescription());
		}

		//add log label
		//m_pLogText = new Widgets::Text(1, "");
		//m_pLogText->SetSizeStyle(Widgets::Widget::HSIZE_STRETCH | Widgets::Widget::VSIZE_STRETCH);
		//pRightPanelLayout->AddWidget(m_pLogText);

		ShaderEditorModule::Get().OnShaderCreated([this](const Systems::AssetMetadata* pMetadata)
			{
				m_pShaderListModel->AddRow(pMetadata);
			});
	}

	void MaterialEditor::Update()
	{}

	void MaterialEditor::Render()
	{}

	void MaterialEditor::CreateMenu(Widgets::Widget* pParent)
	{
		Widgets::MenuBar* pMenuBar = new Widgets::MenuBar();
		pParent->AddWidget(pMenuBar);

		//create the file menu
		{
			Widgets::Menu* pFileMenu = pMenuBar->AddMenu("File");

			Widgets::MenuItem* pNewItem = pFileMenu->AddMenuItem("New Shader...");
			pNewItem->SetShortcut("Ctrl+N");
			pNewItem->OnClick([this]() { MenuFile_NewShader_OnClicked(); });

			Widgets::MenuItem* pSaveItem = pFileMenu->AddMenuItem("Save Shader");
			pSaveItem->SetShortcut("Ctrl+S");
			pSaveItem->OnClick([this]() { MenuFile_Save_OnClicked(); });

			Widgets::MenuItem* pDeleteItem = pFileMenu->AddMenuItem("Delete Shader");
			pDeleteItem->SetShortcut("Del");
			pDeleteItem->OnClick([this]() { MenuFile_Delete_OnClicked(); });
		}
	}

	void MaterialEditor::MenuFile_NewShader_OnClicked()
	{
		//modal windows are automatically deleted when closed,so no need to delete the dialog.
		UserInputDialog* pDialog = new UserInputDialog("New Asset Name");
		pDialog->OnInputValidated([this](const std::string& input) 
			{ 
				Systems::MaterialAsset* pMaterial = ShaderEditorModule::Get().NewShader(input);

				Widgets::SelectionModel* pSelectionModel = m_pShaderListModel->GetSelectionModel();

				int columnCount = m_pShaderListModel->GetColumnCount(Widgets::ModelIndex());
				Widgets::ModelIndex start = m_pShaderListModel->GetIndex(pMaterial->GetId());
				Widgets::ModelIndex end = start.GetSiblingAtColumn(columnCount - 1);
				Widgets::SelectionRow selection(start, end);

				pSelectionModel->SetSelectionRow(selection);
			});
		pDialog->Open();
	}

	void MaterialEditor::MenuFile_Save_OnClicked()
	{
		Widgets::SelectionModel* pSelectionModel = m_pShaderListModel->GetSelectionModel();
		const std::list<Widgets::SelectionRow>& selection = pSelectionModel->GetSelectedRows();
		if (selection.empty())
			return;

		const Widgets::SelectionRow& row = selection.front();
		Systems::NewAssetId id = m_pShaderListModel->GetAssetId(row.GetStartIndex());
		ShaderEditorModule::Get().SaveShader(id);

		m_pShaderListModel->ClearShaderModified(id);
	}

	void MaterialEditor::MenuFile_Delete_OnClicked()
	{
		if (m_selectedMaterialId == Systems::NewAssetId::INVALID)
			return;

		//Ask confirmation
		OkCancelDialog* pDialog = new OkCancelDialog("Delete", "Are you sure you want to delete this material?");
		pDialog->OnOk([this]() { DeleteSelectedShader(); });
		pDialog->Open();
	}

	bool MaterialEditor::OnShaderEntryClicked(Systems::NewAssetId id)
	{
		m_pPropertyGrid->ClearAllItems();

		Systems::ContainerMgr& containerMgr = Systems::ContainerMgr::Get();
		Systems::Container* pContainer = containerMgr.GetContainer(id.GetContainerId());
		if (!pContainer)
		{
			pContainer = containerMgr.LoadContainer(id.GetContainerId());
		}

		if (!pContainer)
			return true;

		Systems::AssetObject* pObject = pContainer->GetAsset(id.GetObjectId());
		if (!pObject)
			return true;

		m_pPropertyGridPopulator->Populate(pObject);

		return true;
	}

	bool MaterialEditor::OnCompileClicked()
	{
		Widgets::SelectionModel* pSelectionModel = m_pShaderListModel->GetSelectionModel();
		const std::list<Widgets::SelectionRow>& selection = pSelectionModel->GetSelectedRows();
		if (selection.empty())
			return true;

		const Widgets::SelectionRow& row = selection.front();
		Systems::NewAssetId id = m_pShaderListModel->GetAssetId(row.GetStartIndex());

		bool res = ShaderEditorModule::Get().CompileShader(id);
		if (!res)
			return true;

		m_pShaderListModel->SetShaderModified(id);

		return true;
	}

	void MaterialEditor::PropertyGridPopulator_OnDataChanged()
	{
		m_pShaderListModel->SetShaderModified(m_selectedMaterialId);
	}

	void MaterialEditor::DeleteSelectedShader()
	{
		if (m_selectedMaterialId == Systems::NewAssetId::INVALID)
			return;

		//first delete it for real
		Systems::AssetUtil::DeleteAsset(m_selectedMaterialId);

		//then delete it from the model
		m_pShaderListModel->RemoveRow(m_selectedMaterialId);
	}

	void MaterialEditor::CreateShadersList()
	{
		Widgets::TableView* pTableView = new Widgets::TableView();
		pTableView->SetSizeStyle(Widgets::Widget::STRETCH);
		m_pShaderListLayout->AddWidget(pTableView);

		m_pShaderListModel = new ShaderListModel();
		pTableView->SetModel(m_pShaderListModel);
		pTableView->SetColumnWidth(ShaderListModel::Columns::Name, 200);

		Widgets::SelectionModel* pSelectionModel = m_pShaderListModel->GetSelectionModel();
		pSelectionModel->OnSelectionChanged([this](const std::vector<Widgets::SelectionRow>& selected, const std::vector<Widgets::SelectionRow>& deselected)
			{
				if (selected.size() == 0)
				{
					m_selectedMaterialId = Systems::NewAssetId::INVALID;
					m_pPropertyGrid->ClearAllItems();
					return;
				}

				const Widgets::SelectionRow& row = selected[0];
				Systems::NewAssetId id = m_pShaderListModel->GetAssetId(row.GetStartIndex());
				if (m_selectedMaterialId == id)
					return;

				m_selectedMaterialId = id;
				OnShaderEntryClicked(id);
			});
	}
}
