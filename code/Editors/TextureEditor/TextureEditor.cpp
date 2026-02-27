/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Editors/TextureEditor/TextureEditor.h"

#include "Core/Log/LogModule.h"
#include "Core/Math/Constants.h"
#include "Core/Math/Mat44f.h"
#include "Core/Math/Sqt.h"

#include "Editors/TextureEditor/TextureEditorModule.h"
#include "Editors/TextureEditor/TextureListModel.h"
#include "Editors/Widgets/Dialog/OkCancelDialog.h"
#include "Editors/Widgets/Dialog/UserInputDialog.h"
#include "Editors/Widgets/PropertyGrid/PropertyGridPopulator.h"
#include "Editors/Widgets/PropertyGrid/PropertyGridWidget.h"

#include "Inputs/InputMgr.h"

#include "OsWin/FileDialog.h"

#include "Rendering/Mesh/MeshMgr.h"
#include "Rendering/PipelineState/PipelineStateMgr.h"
#include "Rendering/Texture/Texture.h"

#include "Systems/Assets/AssetMgr.h"
#include "Systems/Assets/AssetObjects/AssetUtil.h"
#include "Systems/Assets/AssetObjects/Texture/TextureAsset.h"
#include "Systems/Assets/Metadata/AssetMetadata.h"

#include "Widgets/Layout.h"
#include "Widgets/Menu.h"
#include "Widgets/MenuBar.h"
#include "Widgets/MenuItem.h"
#include "Widgets/Models/SelectionModel.h"
#include "Widgets/Models/SelectionRow.h"
#include "Widgets/SplitHorizontal.h"
#include "Widgets/SplitVertical.h"
#include "Widgets/Viewport.h"
#include "Widgets/WidgetMgr.h"
#include "Widgets/Widgets/TableView.h"

//#pragma optimize("", off)

namespace Editors
{
	TextureEditor::TextureEditor()
		: BaseEditor()
		, m_pListModel(nullptr)
		, m_renderTargetHalfWidth(0)
		, m_renderTargetHalfHeight(0)
		, m_scale(512)
		, m_pPopulator(nullptr)
	{ }

	TextureEditor::~TextureEditor()
	{
		delete m_pPopulator;
		m_pPopulator = nullptr;
	}

	void TextureEditor::CreateEditor(Widgets::Widget* pParent)
	{
		CreateDefaultWidgets(pParent, "Texture");

		//create the file menu
		{
			Widgets::Menu* pFileMenu = m_pMenuBar->AddMenu("File");

			Widgets::MenuItem* pDeleteItem = pFileMenu->AddMenuItem("Delete");
			pDeleteItem->SetShortcut("Del");
			pDeleteItem->OnClick([this]() { OnClick_File_Delete(); });

			Widgets::MenuItem* pRenameItem = pFileMenu->AddMenuItem("Rename...");
			pRenameItem->SetShortcut("F2");
			pRenameItem->OnClick([this]() { OnClick_File_Rename(); });
		}

		//create the Texture menu
		{
			Widgets::Menu* pTextureMenu = m_pMenuBar->AddMenu("Texture");

			Widgets::MenuItem* pNewItem = pTextureMenu->AddMenuItem("Import...");
			pNewItem->OnClick([this]() { OnClick_Texture_Import(); });
		}

		//create the cubemap menu
		{
			Widgets::Menu* pCubemapMenu = m_pMenuBar->AddMenu("Cubemap");

			Widgets::MenuItem* pNewCubemapItem = pCubemapMenu->AddMenuItem("Create...");
			pNewCubemapItem->OnClick([this]() { OnClick_Cubemap_NewCubemap(); });
		}

		Widgets::SplitVertical* pVerticalSplit = new Widgets::SplitVertical();
		pVerticalSplit->SetSizeStyle(Widgets::Widget::HSIZE_STRETCH | Widgets::Widget::VSIZE_STRETCH);
		pVerticalSplit->SetLeftPanelWidth(500);

		m_pInternalLayout->AddWidget(pVerticalSplit);

		Widgets::TableView* pTable = new Widgets::TableView();
		pVerticalSplit->AddLeftPanel(pTable);

		m_pListModel = new TextureListModel();
		pTable->SetModel(m_pListModel);
		pTable->SetColumnWidth(TextureListModel::Columns::Name, 200);
		m_pListModel->GetSelectionModel()->OnSelectionChanged([this](const std::vector<Widgets::SelectionRow>& selected, const std::vector<Widgets::SelectionRow>& deselected) 
			{ 
				OnSelectionChanged(selected, deselected); 
			});

		Widgets::SplitHorizontal* pHorizontalSplit = new Widgets::SplitHorizontal();
		pHorizontalSplit->SetSizeStyle(Widgets::Widget::STRETCH);
		pHorizontalSplit->SetTopPanelHeight(800);
		pVerticalSplit->AddRightPanel(pHorizontalSplit);

		const int WIDTH = 1920;
		const int HEIGHT = 1080;

		m_renderTargetHalfWidth = WIDTH * 0.5f;
		m_renderTargetHalfHeight = HEIGHT * 0.5f;

		Widgets::Viewport* pViewport = new Widgets::Viewport(WIDTH, HEIGHT);
		pViewport->OnRender([this]() { Viewport_OnRender(); });
		pViewport->OnUpdate([this](uint64_t dt) { Viewport_OnUpdate(dt); });
		pHorizontalSplit->AddTopPanel(pViewport);

		TextureEditorModule& textureModule = TextureEditorModule::Get();
		textureModule.OnTextureCreated([this](const Systems::AssetMetadata& metadata) { m_pListModel->AddRow(metadata); });
		textureModule.OnBeforeTextureDeleted([this](const Systems::AssetMetadata& metadata) { m_pListModel->RemoveRow(metadata.GetAssetId()); });
		textureModule.OnTextureRenamed([this](const Systems::AssetMetadata& metadata) { m_pListModel->OnTextureRenamed(metadata); });

		PropertyGridWidget* pPropertyGrid = new PropertyGridWidget();
		pHorizontalSplit->AddBottomPanel(pPropertyGrid);

		m_pPopulator = new PropertyGridPopulator();
		m_pPopulator->Init(pPropertyGrid);
	}

	void TextureEditor::OnClick_Texture_Import()
	{
		std::string filename;
		bool res = Os::OpenFileDialog(filename);
		if (!res)
			return;

		res = TextureEditorModule::Get().ImportTexture(filename);

		if (!res)
			Core::LogModule::Get().LogError("Failed to import texture %s", filename.c_str());
		else
			Core::LogModule::Get().LogInfo("Texture %s imported.", filename.c_str());
	}

	void TextureEditor::OnClick_Cubemap_NewCubemap()
	{
		const char* pTitle = "New cubemap name";

		UserInputDialog* pDialog = new UserInputDialog(pTitle);
		pDialog->OnInputValidated([](const std::string& input)
			{
				TextureEditorModule::Get().CreateNewCubemap(input);
			});
		pDialog->Open();
	}

	void TextureEditor::OnClick_File_Delete()
	{
		Systems::NewAssetId selectedTextureId = GetSelectedTextureId();
		if (!selectedTextureId.IsValid())
			return;

		Systems::AssetMetadata* pMetadata = Systems::AssetMgr::Get().GetMetadata(selectedTextureId);
		if (!pMetadata)
			return;

		const int BUFFER_SIZE = 256;
		char buffer[BUFFER_SIZE] = { '\0' };
		snprintf(buffer, BUFFER_SIZE, "Are you sure you want to delete the texture %s?", pMetadata->GetVirtualName().c_str());

		OkCancelDialog* pDialog = new OkCancelDialog("Delete", buffer);
		pDialog->OnOk([selectedTextureId]() { TextureEditorModule::Get().DeleteTexture(selectedTextureId); });
		pDialog->Open();
	}

	void TextureEditor::OnClick_File_Rename()
	{
		Systems::NewAssetId selectedTextureId = GetSelectedTextureId();
		if (!selectedTextureId.IsValid())
			return;

		Systems::AssetMetadata* pMetadata = Systems::AssetMgr::Get().GetMetadata(selectedTextureId);
		if (!pMetadata)
			return;

		const int BUFFER_SIZE = 256;
		char buffer[BUFFER_SIZE] = { '\0' };
		snprintf(buffer, BUFFER_SIZE, "Rename texture %s", pMetadata->GetVirtualName().c_str());

		UserInputDialog* pDialog = new UserInputDialog(buffer);
		pDialog->OnInputValidated([selectedTextureId](const std::string& input)
			{
				TextureEditorModule::Get().RenameTexture(selectedTextureId, input);
			});
		pDialog->Open();
	}

	Systems::NewAssetId TextureEditor::GetSelectedTextureId() const
	{
		const std::list<Widgets::SelectionRow>& selectedRows = m_pListModel->GetSelectionModel()->GetSelectedRows();
		for (const Widgets::SelectionRow& row : selectedRows)
		{
			Widgets::ModelIndex index = row.GetStartIndex();
			return m_pListModel->GetAssetId(index);
		}

		return Systems::NewAssetId::INVALID;
	}

	void TextureEditor::Viewport_OnRender()
	{
		Systems::NewAssetId selectedTextureId = GetSelectedTextureId();
		if (!selectedTextureId.IsValid())
			return;

		bool isATexture = Systems::AssetUtil::IsA<Systems::TextureAsset>(selectedTextureId);
		if (!isATexture)
			return;

		Systems::TextureAsset* pTexture = Systems::AssetUtil::LoadAsset<Systems::TextureAsset>(selectedTextureId);
		if (!pTexture)
			return;

		Core::Vec4f target(0, 0, 10, 1);

		//world, view and proj matrix
		Core::Sqt worldSqt;
		worldSqt.SetTranslation(target);
		worldSqt.SetScale(Core::Vec4f(m_scale, m_scale, 0, 0));
		Core::Mat44f worldTx = Core::Mat44f::CreateTransformMatrix(worldSqt);

		Core::Mat44f viewTx = Core::Mat44f::CreateView(Core::Vec4f(0, 0, 0, 1), target, Core::Vec4f(0, 1, 0, 0));

		Core::Mat44f projTx = Core::Mat44f::CreateOrthographic(-m_renderTargetHalfWidth, m_renderTargetHalfWidth, -m_renderTargetHalfHeight, m_renderTargetHalfHeight, 0.1f, 100.f);

		Core::Mat44f wvp = worldTx * viewTx * projTx;

		//bind material
		Widgets::WidgetMgr& widgetMgr = Widgets::WidgetMgr::Get();
		Rendering::RenderModule& renderer = Rendering::RenderModule::Get();
		Rendering::PipelineStateMgr& psoMgr = Rendering::PipelineStateMgr::Get();

		const Rendering::PipelineState* pPso = psoMgr.GetPipelineState(widgetMgr.GetTextureEditorPsoId());
		renderer.BindMaterial(*pPso);

		//bind const buffer
		renderer.SetConstantBuffer(0, sizeof(wvp), &wvp, 0);

		//bind texture
		ID3D12DescriptorHeap* pSrv = pTexture->GetTexture()->GetSRV();
		ID3D12DescriptorHeap* pDescriptorHeap[] = { pSrv };
		renderer.GetRenderCommandList()->SetDescriptorHeaps(_countof(pDescriptorHeap), pDescriptorHeap);
		renderer.GetRenderCommandList()->SetGraphicsRootDescriptorTable(1, pSrv->GetGPUDescriptorHandleForHeapStart());

		//render mesh
		const Rendering::Mesh* pMesh = Rendering::MeshMgr::Get().GetMesh(widgetMgr.GetQuadMeshId());
		renderer.RenderMesh(*pMesh);
	}

	void TextureEditor::Viewport_OnUpdate(uint64_t dt)
	{
		Inputs::InputMgr& inputs = Inputs::InputMgr::Get();
		int16_t mouseWheelDistance = inputs.GetMouseWheelDistance();
		const float CAMERA_DISTANCE_SPEED = 0.5f;
		const float MIN_SCALE = 20;
		if (mouseWheelDistance != 0)
			m_scale -= mouseWheelDistance * CAMERA_DISTANCE_SPEED;

		if (m_scale < MIN_SCALE)
			m_scale = MIN_SCALE;
	}

	void TextureEditor::OnSelectionChanged(const std::vector<Widgets::SelectionRow>& selected, const std::vector<Widgets::SelectionRow>& deselected)
	{
		if (selected.empty())
		{
			m_pPopulator->Populate(nullptr);
			return;
		}

		const Widgets::SelectionRow& row = selected[0];
		Widgets::ModelIndex index = row.GetStartIndex();
		Systems::NewAssetId assetId = m_pListModel->GetAssetId(index);
		if (!assetId.IsValid())
		{
			m_pPopulator->Populate(nullptr);
			return;
		}

		Systems::AssetObject* pObject = Systems::AssetUtil::LoadAsset(assetId);
		m_pPopulator->Populate(pObject);
	}
}
