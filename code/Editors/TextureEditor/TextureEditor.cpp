/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Editors/TextureEditor/TextureEditor.h"

#include "Core/Log/LogModule.h"
#include "Core/Math/Constants.h"
#include "Core/Math/Mat44f.h"
#include "Core/Math/Sqt.h"

#include "Editors/EditorParameter.h"
#include "Editors/TextureEditor/TextureEditorModule.h"
#include "Editors/TextureEditor/TextureListModel.h"
#include "Editors/Widgets/Dialog/OkCancelDialog.h"
#include "Editors/Widgets/Dialog/UserInputDialog.h"
#include "Editors/Widgets/PropertyGrid/PropertyGridPopulator.h"
#include "Editors/Widgets/PropertyGrid/PropertyGridWidget.h"

#include "Inputs/InputMgr.h"

#include "OsWin/FileDialog.h"

#include "Rendering/BaseShape.h"
#include "Rendering/Mesh/Mesh.h"
#include "Rendering/PipelineState/PipelineStateMgr.h"
#include "Rendering/RootSignature/RootSignature.h"
#include "Rendering/Shaders/Shader.h"
#include "Rendering/Texture/Texture.h"

#include "Systems/Assets/AssetMgr.h"
#include "Systems/Assets/AssetObjects/AssetUtil.h"
#include "Systems/Assets/AssetObjects/Cubemap/Cubemap.h"
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
		, m_cameraDistance(2)
		, m_pPopulator(nullptr)
		, m_objWatcherCid()
		, m_pQuad(nullptr)
		, m_pTextureViewportVertexShader(nullptr)
		, m_pTextureViewportPixelShader(nullptr)
		, m_pRootSig(nullptr)
		, m_pPsoQuad(nullptr)
		, m_pPsoCubemap(nullptr)
		, m_pCube(nullptr)
		, m_aspectRatio()
		, m_viewportState(NONE)
		, m_cameraDirectionRotationY(0)
		, m_firstFrameMouseDown(false)
	{ }

	TextureEditor::~TextureEditor()
	{
		delete m_pPopulator;
		m_pPopulator = nullptr;

		if (m_objWatcherCid.IsValid())
		{
			ObjectWatcher::Get().RemoveWatcher(m_objWatcherCid);
			m_objWatcherCid.Reset();
		}

		delete m_pQuad;
		m_pQuad = nullptr;

		delete m_pTextureViewportVertexShader;
		m_pTextureViewportVertexShader = nullptr;
		delete m_pTextureViewportPixelShader;
		m_pTextureViewportPixelShader = nullptr;
		delete m_pRootSig;
		m_pRootSig = nullptr;
		delete m_pPsoQuad;
		m_pPsoQuad = nullptr;
		delete m_pPsoCubemap;
		m_pPsoCubemap = nullptr;
		delete m_pCube;
		m_pCube = nullptr;
	}

	void TextureEditor::CreateEditor(const EditorParameter& param)
	{
		CreateDefaultWidgets(param.m_pParent, "Texture");

		//create the file menu
		{
			Widgets::Menu* pFileMenu = m_pMenuBar->AddMenu("File");

			Widgets::MenuItem* pSaveItem = pFileMenu->AddMenuItem("Save");
			pSaveItem->SetShortcut("Ctrl+S");
			pSaveItem->OnClick([this]() { OnClick_File_Save(); });

			Widgets::MenuItem* pDeleteItem = pFileMenu->AddMenuItem("Delete");
			pDeleteItem->SetShortcut("Del");
			pDeleteItem->OnClick([this]() { OnClick_File_Delete(); });

			Widgets::MenuItem* pRenameItem = pFileMenu->AddMenuItem("Rename...");
			pRenameItem->SetShortcut("F2");
			pRenameItem->OnClick([this]() { OnClick_File_Rename(); });

			Widgets::MenuItem* pExportItem = pFileMenu->AddMenuItem("Export...");
			pExportItem->OnClick([this]() { OnClick_File_Export(); });
		}

		//create the Texture menu
		{
			Widgets::Menu* pTextureMenu = m_pMenuBar->AddMenu("Texture");

			Widgets::MenuItem* pNewItem = pTextureMenu->AddMenuItem("Create And Import...");
			pNewItem->OnClick([this]() { OnClick_Texture_CreateAndImport(); });

			Widgets::MenuItem* pImportItem = pTextureMenu->AddMenuItem("Import");
			pImportItem->OnClick([this]() { OnClick_Texture_Import(); });
			pImportItem->SetShortcut("Ctrl+F5");
		}

		//create the cubemap menu
		{
			Widgets::Menu* pCubemapMenu = m_pMenuBar->AddMenu("Cubemap");

			Widgets::MenuItem* pNewCubemapItem = pCubemapMenu->AddMenuItem("Create...");
			pNewCubemapItem->OnClick([this]() { OnClick_Cubemap_CreateCubemap(); });

			Widgets::MenuItem* pImportItem = pCubemapMenu->AddMenuItem("Import");
			pImportItem->OnClick([this]() { OnClick_Cubemap_Import(); });
			pImportItem->SetShortcut("Ctrl+F5");
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

		m_aspectRatio = WIDTH / static_cast<float>(HEIGHT);

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
		textureModule.OnTextureSaved([this](const Systems::NewAssetId& id) { m_pListModel->ClearTextureModified(id); });

		PropertyGridWidget* pPropertyGrid = new PropertyGridWidget();
		pHorizontalSplit->AddBottomPanel(pPropertyGrid);

		m_pPopulator = new PropertyGridPopulator();
		m_pPopulator->Init(pPropertyGrid);

		m_pQuad = new Rendering::Mesh();
		Rendering::BaseShape::CreateQuad(m_pQuad);

		m_pCube = new Rendering::Mesh();
		Rendering::BaseShape::CreateCube(m_pCube);

		m_pTextureViewportVertexShader = new Rendering::Shader(param.m_shaderPath + "\\texture_editor_viewport.vs.cso");
		m_pTextureViewportPixelShader = new Rendering::Shader(param.m_shaderPath + "\\texture_editor_viewport.ps.cso");
		m_pRootSig = new Rendering::RootSignature(param.m_shaderPath + "\\texture_editor_viewport.rs.cso");

		m_pPsoQuad = new Rendering::PipelineState();
		m_pPsoQuad->Init_Generic(*m_pRootSig, *m_pTextureViewportVertexShader, *m_pTextureViewportPixelShader, Rendering::PipelineState::BACK);

		m_pPsoCubemap = new Rendering::PipelineState();
		m_pPsoCubemap->Init_Generic(*m_pRootSig, *m_pTextureViewportVertexShader, *m_pTextureViewportPixelShader, Rendering::PipelineState::FRONT);
	}

	void TextureEditor::OnClick_Texture_CreateAndImport()
	{
		std::string filename;
		bool res = Os::OpenFileDialog(filename);
		if (!res)
			return;

		res = TextureEditorModule::Get().CreateAndImportTexture(filename);

		if (!res)
			Core::LogModule::Get().LogError("Failed to import texture %s", filename.c_str());
		else
			Core::LogModule::Get().LogInfo("Texture %s imported.", filename.c_str());
	}

	void TextureEditor::OnClick_Texture_Import()
	{
		Systems::NewAssetId id = GetSelectedTextureId();
		if (!id.IsValid())
			return;

		if (!Systems::AssetUtil::IsA<Systems::TextureAsset>(id))
			return;

		TextureEditorModule::Get().ImportTexture(id);
	}

	void TextureEditor::OnClick_Cubemap_CreateCubemap()
	{
		const char* pTitle = "New cubemap name";

		UserInputDialog* pDialog = new UserInputDialog(pTitle);
		pDialog->OnInputValidated([](const std::string& input)
			{
				TextureEditorModule::Get().CreateCubemap(input);
			});
		pDialog->Open();
	}

	void TextureEditor::OnClick_Cubemap_Import()
	{
		Systems::NewAssetId id = GetSelectedTextureId();
		if (!id.IsValid())
			return;

		if (!Systems::AssetUtil::IsA<Systems::CubemapAsset>(id))
			return;

		TextureEditorModule::Get().ImportTexture(id);
	}

	void TextureEditor::OnClick_File_Save()
	{
		Systems::NewAssetId id = GetSelectedTextureId();
		if (!id.IsValid())
			return;

		bool res = TextureEditorModule::Get().SaveTexture(id);

		if (res)
			Core::LogModule::Get().LogInfo("Texture saved");
		else
			Core::LogModule::Get().LogError("Failed to save texture");
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

	void TextureEditor::OnClick_File_Export()
	{
		Systems::NewAssetId id = GetSelectedTextureId();
		if (!id.IsValid())
		{
			Core::LogModule::Get().LogWarn("No texture selected.");
			return;
		}

		const Systems::AssetMetadata* pMetadata = Systems::AssetMgr::Get().GetMetadata(id);
		if (!pMetadata)
		{
			Core::LogModule::Get().LogWarn("Failed to find metadata.");
			return;
		}

		std::vector<std::string> extensions;
		extensions.push_back("DDS File (*.dds)");
		extensions.push_back("*.dds");
		std::string filename;
		bool res = Os::SaveFileDialog(filename, extensions, pMetadata->GetVirtualName() + ".dds");
		if (!res)
			return;

		res = TextureEditorModule::Get().Export(filename, id);

		if (!res)
			Core::LogModule::Get().LogError("Failed to export.");
		else
			Core::LogModule::Get().LogInfo("Texture exported to %s.", filename.c_str());
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
		bool isACubemap = Systems::AssetUtil::IsA<Systems::CubemapAsset>(selectedTextureId);
		if (!isATexture && !isACubemap)
			return;

		Core::Vec4f target(0, 0, 0, 1);
		Core::Vec4f eye = target + Core::Vec4f(0, 0, -1, 0) * m_cameraDistance;

		//world, view and proj matrix
		Core::Sqt worldSqt;
		worldSqt.SetTranslation(target);
		Core::Mat44f worldTx = Core::Mat44f::CreateTransformMatrix(worldSqt);

		if (m_viewportState == CUBEMAP)
		{
			Core::Mat44f rotation = Core::Mat44f::CreateRotationMatrixFromEulerAngles(Core::Vec4f(0, m_cameraDirectionRotationY, 0, 0));
			Core::Mat44f translation = Core::Mat44f::CreateTranslationMatrix(Core::Vec4f(0, 0, -1, 0) * m_cameraDistance);

			Core::Mat44f transform = translation * rotation;
			eye = target * transform;
		}
		Core::Vec4f cameraDirection = target - eye;
		Core::Mat44f viewTx = Core::Mat44f::CreateView(eye, cameraDirection, Core::Vec4f(0, 1, 0, 0));

		Core::Mat44f projTx = Core::Mat44f::CreatePerspective(45 * 3.14f / 180.f, m_aspectRatio, 0.1f, 100);

		Core::Mat44f wvp = worldTx * viewTx * projTx;

		Rendering::RenderModule& renderer = Rendering::RenderModule::Get();

		const int ROOT_SIG_INDEX_CBV_VERTEX_SHADER = 0;
		const int ROOT_SIG_INDEX_CBV_PIXEL_SHADER = 1;
		const int ROOT_SIG_INDEX_TEXTURE2D = 2;
		const int ROOT_SIG_INDEX_TEXTURECUBE = 3;
		const int ROOT_SIG_INDEX_STATICSAMPLER = 4;

		struct PixelShaderCBuffer
		{
			Core::Mat44f world;
			uint32_t type;
		};

		struct VertexShaderCBuffer
		{
			Core::Mat44f world;
			Core::Mat44f wvp;
		};

		VertexShaderCBuffer vsCBuffer;
		vsCBuffer.world = worldTx;
		vsCBuffer.wvp = wvp;

		if (isATexture)
		{
			Systems::TextureAsset* pTexture = Systems::AssetUtil::GetAsset<Systems::TextureAsset>(selectedTextureId);
			if (!pTexture)
				return;

			renderer.BindMaterial(*m_pPsoQuad, *m_pRootSig);

			PixelShaderCBuffer psCBuffer;
			psCBuffer.world = worldTx;
			psCBuffer.type = 0;

			//bind const buffer
			renderer.SetConstantBuffer(ROOT_SIG_INDEX_CBV_VERTEX_SHADER, sizeof(vsCBuffer), &vsCBuffer, 0);
			renderer.SetConstantBuffer(ROOT_SIG_INDEX_CBV_PIXEL_SHADER, sizeof(psCBuffer), &psCBuffer, 0);

			//bind texture
			ID3D12DescriptorHeap* pSrv = pTexture->GetTexture()->GetSRV();
			ID3D12DescriptorHeap* pDescriptorHeap[] = { pSrv };
			renderer.GetRenderCommandList()->SetDescriptorHeaps(_countof(pDescriptorHeap), pDescriptorHeap);
			renderer.GetRenderCommandList()->SetGraphicsRootDescriptorTable(ROOT_SIG_INDEX_TEXTURE2D, pSrv->GetGPUDescriptorHandleForHeapStart());

			renderer.BindNullCubemap(ROOT_SIG_INDEX_TEXTURECUBE);

			//render mesh
			renderer.RenderMesh(*m_pQuad);
		}
		else
		{
			Systems::CubemapAsset* pTexture = Systems::AssetUtil::GetAsset<Systems::CubemapAsset>(selectedTextureId);
			if (!pTexture)
				return;

			renderer.BindMaterial(*m_pPsoCubemap, *m_pRootSig);

			PixelShaderCBuffer psCBuffer;
			psCBuffer.world = worldTx;
			psCBuffer.type = 1;

			//bind const buffer
			renderer.SetConstantBuffer(ROOT_SIG_INDEX_CBV_VERTEX_SHADER, sizeof(vsCBuffer), &vsCBuffer, 0);
			renderer.SetConstantBuffer(ROOT_SIG_INDEX_CBV_PIXEL_SHADER, sizeof(psCBuffer), &psCBuffer, 0);

			//bind texture
			ID3D12DescriptorHeap* pSrv = pTexture->GetTexture()->GetSRV();
			ID3D12DescriptorHeap* pDescriptorHeap[] = { pSrv };
			renderer.GetRenderCommandList()->SetDescriptorHeaps(_countof(pDescriptorHeap), pDescriptorHeap);
			renderer.GetRenderCommandList()->SetGraphicsRootDescriptorTable(ROOT_SIG_INDEX_TEXTURECUBE, pSrv->GetGPUDescriptorHandleForHeapStart());

			renderer.BindNullTexture2D(ROOT_SIG_INDEX_TEXTURE2D);

			//render mesh
			renderer.RenderMesh(*m_pCube);
		}
	}

	void TextureEditor::Viewport_OnUpdate(uint64_t dt)
	{
		switch (m_viewportState)
		{
		case TEXTURE:
		{
			Inputs::InputMgr& inputs = Inputs::InputMgr::Get();
			int16_t mouseWheelDistance = inputs.GetMouseWheelDistance();
			const float CAMERA_DISTANCE_SPEED = 0.002f;
			const float MIN_SCALE = 1;
			if (mouseWheelDistance != 0)
				m_cameraDistance -= mouseWheelDistance * CAMERA_DISTANCE_SPEED;

			if (m_cameraDistance < MIN_SCALE)
				m_cameraDistance = MIN_SCALE;
		}
		break;

		case CUBEMAP:
		{
			Inputs::InputMgr& inputs = Inputs::InputMgr::Get();
			int16_t mouseWheelDistance = inputs.GetMouseWheelDistance();
			const float CAMERA_DISTANCE_SPEED = 0.002f;
			const float MIN_SCALE = 1;
			if (mouseWheelDistance != 0)
				m_cameraDistance -= mouseWheelDistance * CAMERA_DISTANCE_SPEED;

			if (m_cameraDistance < MIN_SCALE)
				m_cameraDistance = MIN_SCALE;

			if (inputs.IsMouseLeftButtonDown())
			{
				Core::UInt2 mousePosition;
				inputs.GetMousePosition(mousePosition.x, mousePosition.y);
				if (m_firstFrameMouseDown)
				{
					m_mousePreviousPos = mousePosition;
					m_firstFrameMouseDown = false;
				}

				DirectX::XMINT2 delta;
				delta.x = m_mousePreviousPos.x - mousePosition.x;
				delta.y = m_mousePreviousPos.y - mousePosition.y;

				const float ROTATION_SPEED = 0.01f;
				m_cameraDirectionRotationY += static_cast<float>(delta.x) * ROTATION_SPEED;
				//DirectX::XMVECTOR offset = DirectX::XMVectorSet(static_cast<float>(delta.y) * ROTATION_SPEED, -static_cast<float>(delta.x) * ROTATION_SPEED, 0, 0);

				//m_cameraEuler = DirectX::XMVectorAdd(m_cameraEuler, offset);

				m_mousePreviousPos = mousePosition;
			}
			else if (!m_firstFrameMouseDown)
			{
				m_firstFrameMouseDown = true;
			}
		}
		break;

		case NONE:
		default:
			break;
		}
	}

	void TextureEditor::OnSelectionChanged(const std::vector<Widgets::SelectionRow>& selected, const std::vector<Widgets::SelectionRow>& deselected)
	{
		if (m_objWatcherCid.IsValid())
		{
			ObjectWatcher::Get().RemoveWatcher(m_objWatcherCid);
			m_objWatcherCid.Reset();
		}

		if (selected.empty())
		{
			m_pPopulator->Populate(nullptr);
			m_viewportState = NONE;
			return;
		}

		const Widgets::SelectionRow& row = selected[0];
		Widgets::ModelIndex index = row.GetStartIndex();
		Systems::NewAssetId assetId = m_pListModel->GetAssetId(index);
		if (!assetId.IsValid())
		{
			m_pPopulator->Populate(nullptr);
			m_viewportState = NONE;
			return;
		}

		Systems::AssetObject* pObject = Systems::AssetUtil::LoadAsset(assetId);
		m_pPopulator->Populate(pObject);

		if(pObject->IsA<Systems::TextureAsset>())
			m_viewportState = TEXTURE;
		else if (pObject->IsA<Systems::CubemapAsset>())
			m_viewportState = CUBEMAP;
		else
			m_viewportState = NONE;

		m_objWatcherCid = ObjectWatcher::Get().AddWatcher(pObject, [this](void*, const Systems::FieldDescriptor*, ObjectWatcher::OPERATION, uint32_t) { OnDataChanged(); });
	}

	void TextureEditor::OnDataChanged()
	{
		m_pListModel->SetTextureModified(GetSelectedTextureId());
	}
}
