/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Editors/ParticleEditor/ParticleEditor.h"

#include "Core/Math/Constants.h"
#include "Core/Math/Mat44f.h"

#include "Editors/EditorParameter.h"
#include "Editors/ParticleEditor/ParticleEditorModule.h"
#include "Editors/ParticleEditor/ParticleListModel.h"
#include "Editors/Widgets/Camera/CameraWidget.h"
#include "Editors/Widgets/Dialog/OkCancelDialog.h"
#include "Editors/Widgets/Dialog/UserInputDialog.h"
#include "Editors/Widgets/PropertyGrid/PropertyGridPopulator.h"
#include "Editors/Widgets/PropertyGrid/PropertyGridWidget.h"

#include "Rendering/PipelineState/PipelineStateDesc.h"
#include "Rendering/RenderModule.h"
#include "Rendering/RenderTargets/RenderTarget.h"
#include "Rendering/RootSignature/RootSignatureMgr.h"
#include "Rendering/Shaders/ShaderMgr.h"
#include "Rendering/Texture/Texture.h"

#include "Systems/Assets/AssetMgr.h"
#include "Systems/Assets/AssetObjects/AssetUtil.h"
#include "Systems/Assets/AssetObjects/ParticleEffect/ParticleEffectAsset.h"
#include "Systems/Clock/Clock.h"
#include "Systems/Game/GameContext.h"
#include "Systems/Game/Subsystems/Clock/IClockSubsystem.h"
#include "Systems/Rendering/Renderable/RenderableScene.h"
#include "Systems/Rendering/RenderPass/RenderPassBase.h"

#include "Widgets/Layout.h"
#include "Widgets/Menu.h"
#include "Widgets/MenuBar.h"
#include "Widgets/MenuItem.h"
#include "Widgets/Models/SelectionModel.h"
#include "Widgets/Models/SelectionRow.h"
#include "Widgets/SplitHorizontal.h"
#include "Widgets/SplitVertical.h"
#include "Widgets/Widgets/TableView.h"
#include "Widgets/Widgets/Viewport.h"

namespace Editors
{
	ParticleEditor::ParticleEditor()
		: BaseEditor()
		, m_pPopulator(nullptr)
		, m_pListModel(nullptr)
		, m_pBasePass(nullptr)
		, m_pViewport(nullptr)
		, m_pCopyPso(nullptr)
		, m_pCopyRootSig(nullptr)
		, m_aspectRatio()
		, m_pCamera(nullptr)
		, clock(0)
	{ }

	ParticleEditor::~ParticleEditor()
	{
		delete m_pPopulator;
		delete m_pBasePass;
		delete m_pCopyPso;
		delete m_pCamera;
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
		pTable->SetColumnWidth(ParticleListModel::Columns::Id, 130);
		pTable->SetColumnWidth(ParticleListModel::Columns::Name, 300);

		pVerticalSplit->AddLeftPanel(pTable);

		Widgets::SplitHorizontal* pHorizontalSplit = new Widgets::SplitHorizontal();
		pHorizontalSplit->SetSizeStyle(Widgets::STRETCH);
		pHorizontalSplit->SetTopPanelHeight(600);
		pVerticalSplit->AddRightPanel(pHorizontalSplit);

		const int WIDTH = 1920;
		const int HEIGHT = 1080;
		m_aspectRatio = WIDTH / static_cast<float>(HEIGHT);

		m_pViewport = new Widgets::Viewport(WIDTH, HEIGHT, true);
		m_pViewport->OnRender([this]() { Viewport_OnRender(); });
		m_pViewport->OnUpdate([this](uint64_t dt) { Viewport_OnUpdate(dt); });
		pHorizontalSplit->AddTopPanel(m_pViewport);

		PropertyGridWidget* pPropertyGrid = new PropertyGridWidget();
		pHorizontalSplit->AddBottomPanel(pPropertyGrid);

		m_pPopulator = new PropertyGridPopulator();
		m_pPopulator->Init(pPropertyGrid);

		ParticleEditorModule& module = ParticleEditorModule::Get();
		module.OnParticleEffectCreated([this](const Systems::AssetMetadata& metadata) { m_pListModel->AddRow(metadata); });
		module.OnBeforeParticleEffectDeleted([this](Systems::NewAssetId id) { m_pListModel->RemoveRow(id); });
		module.OnParticleEffectSaved([this](Systems::NewAssetId id) { m_pListModel->ClearModifiedMark(id); });
		module.OnParticleEffectRenamed([this](Systems::AssetMetadata& metadata) { m_pListModel->OnParticleEffectRenamed(metadata); });

		m_pBasePass = new Systems::RenderPassBase(WIDTH, HEIGHT);

		//pso and rootsig to copy to the viewport
		m_pCopyRootSig = Rendering::RootSignatureMgr::Get().GetRootSignature(Rendering::EngineRootSigs::COPY_RENDER_TARGET);

		Rendering::PipelineStateDesc desc;
		desc.m_depthStencilDesc.m_depthFunction = Rendering::DepthComparisonMode::ALWAYS;
		desc.m_cullMode = Rendering::CullMode::NONE;
		desc.m_pVs = Rendering::ShaderMgr::Get().GetShader(Rendering::EngineShaders::COPY_RENDER_TARGET_VS);
		desc.m_pPs = Rendering::ShaderMgr::Get().GetShader(Rendering::EngineShaders::COPY_RENDER_TARGET_PS);
		desc.m_pRs = m_pCopyRootSig;

		m_pCopyPso = new Rendering::PipelineState();
		m_pCopyPso->Init_Generic(desc);

		m_pCamera = new CameraWidget();
		m_pCamera->SetFov(45 * Core::PI_OVER_180);
		m_pCamera->SetAspectRatio(m_aspectRatio);
		m_pCamera->SetTransform(Core::Vec4f(0, 0, -20, 1), Core::Vec4f(0, 0, 0, 0));
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

		{
			const Systems::ParticleEmitter& emitter = pEffect->GetEmitter();
			ObjectWatcherCallbackId id = ObjectWatcher::Get().AddWatcher(&emitter, [this](void*, const Core::FieldDescriptor*, ObjectWatcher::OPERATION, uint32_t) { OnParticleEffectModified(); });
			m_objWatcherCid.PushBack(id);
		}

		Systems::GameContext* pContext = ParticleEditorModule::Get().GetGameContext();
		pContext->m_pParticleSystem->KillAllEffect();
		pContext->m_pParticleSystem->SpawnEffect(pEffect, Core::Mat44f::CreateIdentity(), clock);
	}

	void ParticleEditor::OnParticleEffectModified()
	{
		Systems::NewAssetId id = m_pListModel->GetSelection();
		m_pListModel->SetModifiedMark(id);

		Systems::ParticleEffectAsset* pEffect = Systems::AssetUtil::LoadAsset<Systems::ParticleEffectAsset>(id, Systems::LoadingDomain::EDITOR);
		if (pEffect)
		{
			Systems::GameContext* pContext = ParticleEditorModule::Get().GetGameContext();

			pContext->m_pParticleSystem->KillAllEffect();
			pContext->m_pParticleSystem->SpawnEffect(pEffect, Core::Mat44f::CreateIdentity(), Systems::Clock::Get().GetApplicationTime());
		}
	}

	void ParticleEditor::Viewport_OnUpdate(uint64_t dt)
	{
		if (!m_pViewport->IsEnabled())
			return;

		float dtInSeconds = dt / 1000.f;
		clock += dtInSeconds;

		m_pCamera->Update(dtInSeconds);

		Systems::GameContext* pContext = ParticleEditorModule::Get().GetGameContext();
		pContext->m_pClock->Update(dtInSeconds);
		pContext->m_pParticleSystem->Update(*pContext);
	}

	void ParticleEditor::Viewport_OnRender()
	{
		Systems::RenderableScene scene;
		scene.m_camera.m_fov = m_aspectRatio;
		scene.m_camera.m_position = m_pCamera->GetPosition();
		scene.m_camera.m_view = m_pCamera->GetView();
		scene.m_camera.m_proj = m_pCamera->GetProjection();

		//first make renderable
		Systems::GameContext* pContext = ParticleEditorModule::Get().GetGameContext();
		pContext->m_pParticleSystem->BuildRenderable(scene);
		
		//then render the base pass
		m_pBasePass->PreRender(scene);
		m_pBasePass->Render(scene);
		m_pBasePass->PostRender(scene);

		//finally show it to the viewport
		m_pViewport->BeginScene();

		{
			Rendering::RenderTarget* pFinalRT = m_pBasePass->GetRenderTarget();
			Rendering::Texture* pFinalTexture = pFinalRT->GetColorTexture();
			pFinalTexture->TransitionToShaderResource();

			Rendering::RenderModule& renderer = Rendering::RenderModule::Get();
			renderer.BindMaterial(*m_pCopyPso, *m_pCopyRootSig);

			{
				ID3D12DescriptorHeap* pSrv = pFinalTexture->GetSRV();
				ID3D12DescriptorHeap* pDescriptorHeap[] = { pSrv };

				//I should have only 2 giant descriptor heaps, one for cbv,srv,uav and another one for sampler.
				renderer.GetRenderCommandList()->SetDescriptorHeaps(_countof(pDescriptorHeap), pDescriptorHeap);
				renderer.GetRenderCommandList()->SetGraphicsRootDescriptorTable(0, pSrv->GetGPUDescriptorHandleForHeapStart());
			}

			renderer.RenderNoBufferTriangle();
		}

		m_pViewport->EndScene();
	}

	void ParticleEditor::RemoveAllWatchers()
	{
		for(ObjectWatcherCallbackId id : m_objWatcherCid)
			ObjectWatcher::Get().RemoveWatcher(id);

		m_objWatcherCid.Clear();
	}
}
