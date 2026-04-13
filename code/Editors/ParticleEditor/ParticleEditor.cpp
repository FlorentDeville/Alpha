/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Editors/ParticleEditor/ParticleEditor.h"

#include "Editors/EditorParameter.h"
#include "Editors/Widgets/PropertyGrid/PropertyGridPopulator.h"
#include "Editors/Widgets/PropertyGrid/PropertyGridWidget.h"

#include "Widgets/Layout.h"
#include "Widgets/Menu.h"
#include "Widgets/MenuBar.h"
#include "Widgets/MenuItem.h"
#include "Widgets/SplitHorizontal.h"
#include "Widgets/SplitVertical.h"
#include "Widgets/Viewport.h"
#include "Widgets/Widgets/TableView.h"

namespace Editors
{
	ParticleEditor::ParticleEditor()
		: BaseEditor()
		, m_pPopulator(nullptr)
	{ }

	ParticleEditor::~ParticleEditor()
	{
		delete m_pPopulator;
	}

	void ParticleEditor::CreateEditor(const EditorParameter& param)
	{
		CreateDefaultWidgets(param.m_pParent, "Particles");

		//create the file menu
		{
			Widgets::Menu* pFileMenu = m_pMenuBar->AddMenu("File");

			Widgets::MenuItem* pSaveItem = pFileMenu->AddMenuItem("Save");
			pSaveItem->SetShortcut("Ctrl+S");

			Widgets::MenuItem* pDeleteItem = pFileMenu->AddMenuItem("Delete");
			pDeleteItem->SetShortcut("Del");

			Widgets::MenuItem* pRenameItem = pFileMenu->AddMenuItem("Rename...");
			pRenameItem->SetShortcut("F2");
		}

		Widgets::SplitVertical* pVerticalSplit = new Widgets::SplitVertical();
		pVerticalSplit->SetSizeStyle(Widgets::HSIZE_STRETCH | Widgets::VSIZE_STRETCH);
		pVerticalSplit->SetLeftPanelWidth(500);

		m_pInternalLayout->AddWidget(pVerticalSplit);

		Widgets::TableView* pTable = new Widgets::TableView();
		pVerticalSplit->AddLeftPanel(pTable);

		/*m_pListModel = new TextureListModel();
		pTable->SetModel(m_pListModel);
		pTable->SetColumnWidth(TextureListModel::Columns::Name, 200);
		pTable->SetColumnWidth(TextureListModel::Columns::Modified, 200);
		m_pListModel->GetSelectionModel()->OnSelectionChanged([this](const std::vector<Widgets::SelectionRow>& selected, const std::vector<Widgets::SelectionRow>& deselected)
			{
				OnSelectionChanged(selected, deselected);
			});*/

		Widgets::SplitHorizontal* pHorizontalSplit = new Widgets::SplitHorizontal();
		pHorizontalSplit->SetSizeStyle(Widgets::STRETCH);
		pHorizontalSplit->SetTopPanelHeight(600);
		pVerticalSplit->AddRightPanel(pHorizontalSplit);

		const int WIDTH = 1920;
		const int HEIGHT = 1080;

		/*m_aspectRatio = WIDTH / static_cast<float>(HEIGHT);

		m_renderTargetHalfWidth = WIDTH * 0.5f;
		m_renderTargetHalfHeight = HEIGHT * 0.5f;*/

		Widgets::Viewport* pViewport = new Widgets::Viewport(WIDTH, HEIGHT);
		//pViewport->OnRender([this]() { Viewport_OnRender(); });
		//pViewport->OnUpdate([this](uint64_t dt) { Viewport_OnUpdate(dt); });
		pHorizontalSplit->AddTopPanel(pViewport);

		PropertyGridWidget* pPropertyGrid = new PropertyGridWidget();
		pHorizontalSplit->AddBottomPanel(pPropertyGrid);

		m_pPopulator = new PropertyGridPopulator();
		m_pPopulator->Init(pPropertyGrid);
	}
}
