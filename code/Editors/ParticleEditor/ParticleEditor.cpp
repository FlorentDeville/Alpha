/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Editors/ParticleEditor/ParticleEditor.h"

#include "Editors/EditorParameter.h"
#include "Editors/ParticleEditor/ParticleEditorModule.h"
#include "Editors/ParticleEditor/ParticleListModel.h"
#include "Editors/Widgets/Dialog/OkCancelDialog.h"
#include "Editors/Widgets/Dialog/UserInputDialog.h"
#include "Editors/Widgets/PropertyGrid/PropertyGridPopulator.h"
#include "Editors/Widgets/PropertyGrid/PropertyGridWidget.h"

#include "Systems/Assets/AssetMgr.h"
#include "Systems/Assets/AssetObjects/AssetUtil.h"
#include "Systems/Assets/AssetObjects/ParticleEffect/ParticleEffectAsset.h"

#include "Widgets/Layout.h"
#include "Widgets/Menu.h"
#include "Widgets/MenuBar.h"
#include "Widgets/MenuItem.h"
#include "Widgets/Models/SelectionModel.h"
#include "Widgets/Models/SelectionRow.h"
#include "Widgets/SplitHorizontal.h"
#include "Widgets/SplitVertical.h"
#include "Widgets/Viewport.h"
#include "Widgets/Widgets/TableView.h"

namespace Editors
{
	ParticleEditor::ParticleEditor()
		: BaseEditor()
		, m_pPopulator(nullptr)
		, m_pListModel(nullptr)
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

			Widgets::MenuItem* pCreateItem = pFileMenu->AddMenuItem("Create...");
			pCreateItem->SetShortcut("Ctrl+N");
			pCreateItem->OnClick([this]() { OnMenu_File_Create(); });

			Widgets::MenuItem* pSaveItem = pFileMenu->AddMenuItem("Save");
			pSaveItem->SetShortcut("Ctrl+S");
			pSaveItem->OnClick([this]() {OnMenu_File_Save(); });

			Widgets::MenuItem* pDeleteItem = pFileMenu->AddMenuItem("Delete");
			pDeleteItem->SetShortcut("Del");
			pDeleteItem->OnClick([this]() { OnMenu_File_Delete(); });

			Widgets::MenuItem* pRenameItem = pFileMenu->AddMenuItem("Rename...");
			pRenameItem->SetShortcut("F2");
			pRenameItem->OnClick([this]() { OnMenu_File_Rename(); });
		}

		Widgets::SplitVertical* pVerticalSplit = new Widgets::SplitVertical();
		pVerticalSplit->SetSizeStyle(Widgets::HSIZE_STRETCH | Widgets::VSIZE_STRETCH);
		pVerticalSplit->SetLeftPanelWidth(500);

		m_pInternalLayout->AddWidget(pVerticalSplit);

		m_pListModel = new ParticleListModel();
		m_pListModel->GetSelectionModel()->OnSelectionChanged([this](const std::vector<Widgets::SelectionRow>& selected, const std::vector<Widgets::SelectionRow>& deselected) 
			{
				OnSelectionChanged(selected, deselected);
			});
		Widgets::TableView* pTable = new Widgets::TableView();
		pTable->SetModel(m_pListModel);

		pVerticalSplit->AddLeftPanel(pTable);

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

		ParticleEditorModule& module = ParticleEditorModule::Get();
		module.OnParticleEffectCreated([this](const Systems::AssetMetadata& metadata) { m_pListModel->AddRow(metadata); });
		module.OnBeforeParticleEffectDeleted([this](Systems::NewAssetId id) { m_pListModel->RemoveRow(id); });
		module.OnParticleEffectSaved([this](Systems::NewAssetId id) { m_pListModel->ClearModifiedMark(id); });
		module.OnParticleEffectRenamed([this](Systems::AssetMetadata& metadata) { m_pListModel->OnParticleEffectRenamed(metadata); });
	}

	void ParticleEditor::OnMenu_File_Create()
	{
		const char* pTitle = "New particle effect name";

		UserInputDialog* pDialog = new UserInputDialog(pTitle);
		pDialog->OnInputValidated([this](const std::string& input)
			{
				Systems::ParticleEffectAsset* pEffect = ParticleEditorModule::Get().CreateParticleEffect(input);
				if (pEffect)
					m_pListModel->SetSelection(pEffect->GetId());
			});
		pDialog->Open();
	}

	void ParticleEditor::OnMenu_File_Delete()
	{
		Systems::NewAssetId selectedEffectId = m_pListModel->GetSelection();
		if (!selectedEffectId.IsValid())
			return;

		Systems::AssetMetadata* pMetadata = Systems::AssetMgr::Get().GetMetadata(selectedEffectId);
		if (!pMetadata)
			return;

		const int BUFFER_SIZE = 256;
		char buffer[BUFFER_SIZE] = { '\0' };
		snprintf(buffer, BUFFER_SIZE, "Are you sure you want to delete the effect %s?", pMetadata->GetVirtualName().c_str());

		OkCancelDialog* pDialog = new OkCancelDialog("Delete", buffer);
		pDialog->OnOk([selectedEffectId]() { ParticleEditorModule::Get().DeleteParticleEffect(selectedEffectId); });
		pDialog->Open();
	}

	void ParticleEditor::OnMenu_File_Save()
	{
		Systems::NewAssetId selectedEffectId = m_pListModel->GetSelection();
		if (!selectedEffectId.IsValid())
			return;

		ParticleEditorModule::Get().SaveParticleEffect(selectedEffectId);
	}

	void ParticleEditor::OnMenu_File_Rename()
	{
		Systems::NewAssetId selectedEffectId = m_pListModel->GetSelection();
		if (!selectedEffectId.IsValid())
			return;

		Systems::AssetMetadata* pMetadata = Systems::AssetMgr::Get().GetMetadata(selectedEffectId);
		if (!pMetadata)
			return;

		const int BUFFER_SIZE = 256;
		char buffer[BUFFER_SIZE] = { '\0' };
		snprintf(buffer, BUFFER_SIZE, "Rename effect %s?", pMetadata->GetVirtualName().c_str());

		UserInputDialog* pDialog = new UserInputDialog(buffer);
		pDialog->OnInputValidated([selectedEffectId](const std::string& input)
			{
				ParticleEditorModule::Get().RenameParticleEffect(selectedEffectId, input);
			});
		pDialog->Open();
	}

	void ParticleEditor::OnSelectionChanged(const std::vector<Widgets::SelectionRow>& selected, const std::vector<Widgets::SelectionRow>& deselected)
	{
		if (selected.empty())
		{
			RemoveAllWatchers();
			m_pPopulator->Populate(nullptr);
			return;
		}

		const Widgets::SelectionRow& row = selected.back();
		Systems::NewAssetId id = m_pListModel->GetAssetId(row.GetStartIndex());

		if (!id.IsValid())
		{
			RemoveAllWatchers();
			m_pPopulator->Populate(nullptr);
			return;
		}

		Systems::ParticleEffectAsset* pEffect = Systems::AssetUtil::LoadAsset<Systems::ParticleEffectAsset>(id, Systems::LoadingDomain::EDITOR);
		if (!pEffect)
		{
			RemoveAllWatchers();
			m_pPopulator->Populate(nullptr);
			return;
		}

		m_pPopulator->Populate(pEffect);
		ObjectWatcherCallbackId cid = ObjectWatcher::Get().AddWatcher(pEffect, [this](void*, const Core::FieldDescriptor*, ObjectWatcher::OPERATION, uint32_t) { OnParticleEffectModified(); });
		m_objWatcherCid.PushBack(cid);

		for (const Systems::ParticleEmitter* pEmitter : pEffect->GetEmitters())
		{
			ObjectWatcherCallbackId id = ObjectWatcher::Get().AddWatcher(pEmitter, [this](void*, const Core::FieldDescriptor*, ObjectWatcher::OPERATION, uint32_t) { OnParticleEffectModified(); });
			m_objWatcherCid.PushBack(id);
		}
	}

	void ParticleEditor::OnParticleEffectModified()
	{
		Systems::NewAssetId id = m_pListModel->GetSelection();
		m_pListModel->SetModifiedMark(id);
	}

	void ParticleEditor::RemoveAllWatchers()
	{
		for(ObjectWatcherCallbackId id : m_objWatcherCid)
			ObjectWatcher::Get().RemoveWatcher(id);

		m_objWatcherCid.Clear();
	}
}
