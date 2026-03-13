/********************************************************************************/
/* Copyright (C) 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Editors/MeshEditor/MeshEditor.h"

#include "Core/Log/LogModule.h"
#include "Core/Math/Constants.h"

#include "Editors/EditorParameter.h"
#include "Editors/MeshEditor/MeshEditorModule.h"
#include "Editors/MeshEditor/MeshListModel.h"
#include "Editors/Widgets/Dialog/OkCancelDialog.h"
#include "Editors/Widgets/Dialog/UserInputDialog.h"
#include "Editors/Widgets/PropertyGrid/PropertyGridPopulator.h"
#include "Editors/Widgets/PropertyGrid/PropertyGridWidget.h"

#include "Inputs/InputMgr.h"

#include "OsWin/FileDialog.h"

#include "Rendering/Camera.h"
#include "Rendering/ConstantBuffer/LightsCBuffer.h"
#include "Rendering/ConstantBuffer/PerFrameCBuffer.h"
#include "Rendering/ConstantBuffer/PerObjectCBuffer.h"
#include "Rendering/RenderModule.h"
#include "Rendering/RenderTargets/RenderTarget.h"
#include "Rendering/Texture/Texture.h"

#include "Systems/Assets/AssetObjects/AssetUtil.h"
#include "Systems/Assets/AssetObjects/Material/MaterialAsset.h"
#include "Systems/Assets/AssetObjects/Mesh/MeshAsset.h"
#include "Systems/Rendering/MaterialRendering.h"

#include "Widgets/Menu.h"
#include "Widgets/MenuBar.h"
#include "Widgets/MenuItem.h"
#include "Widgets/Models/SelectionModel.h"
#include "Widgets/Models/SelectionRow.h"
#include "Widgets/Label.h"
#include "Widgets/Layout.h"
#include "Widgets/SplitHorizontal.h"
#include "Widgets/SplitVertical.h"
#include "Widgets/Tab.h"
#include "Widgets/TabContainer.h"
#include "Widgets/Text.h"
#include "Widgets/Viewport.h"
#include "Widgets/Widgets/TableView.h"

namespace Editors
{
	MaterialEntry::MaterialEntry()
		: m_Id()
	{}

	MeshEditor::MeshEditor()
		: BaseEditor()
		, m_cameraDistance(10.f)
		, m_materialId()
		, m_aspectRatio(0.f)
		, m_firstFrameMouseDown(true)
		, m_mousePreviousPos(0, 0)
		, m_allEntryButton()
		, m_allMaterials()
		, m_pSelectedMesh(nullptr)
		, m_pMeshListModel(nullptr)
		, m_pPopulator(nullptr)
		, m_pWorldAxisRenderTarget(nullptr)
	{
		m_cameraEuler = Core::Vec4f(0, 0, 0, 1);
		m_cameraTarget = Core::Vec4f(0, 0, 0, 1);
	}

	MeshEditor::~MeshEditor()
	{
		delete m_pPopulator;
		m_pPopulator = nullptr;

		delete m_pWorldAxisRenderTarget;
		m_pWorldAxisRenderTarget = nullptr;
	}

	void MeshEditor::CreateEditor(const EditorParameter& param)
	{
		CreateDefaultWidgets(param.m_pParent, "Mesh");

		//create the file menu
		{
			Widgets::Menu* pFileMenu = m_pMenuBar->AddMenu("File");

			Widgets::MenuItem* pNewItem = pFileMenu->AddMenuItem("Import...");
			pNewItem->OnClick([this]() { OnClicked_File_Import(); });

			Widgets::MenuItem* pReimportItem = pFileMenu->AddMenuItem("Reimport");
			pReimportItem->SetShortcut("Ctrl+F5");
			pReimportItem->OnClick([this]() { OnClicked_File_Reimport(); });

			Widgets::MenuItem* pSaveItem = pFileMenu->AddMenuItem("Save");
			pSaveItem->SetShortcut("Ctrl+S");
			pSaveItem->OnClick([this]() { OnClicked_File_Save(); });

			Widgets::MenuItem* pRenameItem = pFileMenu->AddMenuItem("Rename...");
			pRenameItem->SetShortcut("F2");
			pRenameItem->OnClick([this]() { OnClicked_File_Rename(); });

			Widgets::MenuItem* pDeleteItem = pFileMenu->AddMenuItem("Delete...");
			pDeleteItem->SetShortcut("Del");
			pDeleteItem->OnClick([this]() { OnClicked_File_Delete(); });
		}

		//create the split
		Widgets::SplitVertical* pSplit = new Widgets::SplitVertical();
		pSplit->SetSizeStyle(Widgets::Widget::HSIZE_STRETCH | Widgets::Widget::VSIZE_STRETCH);
		pSplit->SetLeftPanelWidth(400);
		m_pInternalLayout->AddWidget(pSplit);

		//create right side vertical split
		Widgets::SplitHorizontal* pRightSideSplit = new Widgets::SplitHorizontal();
		pRightSideSplit->SetSizeStyle(Widgets::Widget::HSIZE_STRETCH | Widgets::Widget::VSIZE_STRETCH);
		pRightSideSplit->SetTopPanelHeight(700);
		pSplit->AddRightPanel(pRightSideSplit);
		
		//create left viewport
		const int VIEWPORT_WIDTH = 1280;
		const int VIEWPORT_HEIGHT = 720;
		m_aspectRatio = VIEWPORT_WIDTH / static_cast<float>(VIEWPORT_HEIGHT);
		Widgets::Viewport* pViewport = new Widgets::Viewport(VIEWPORT_WIDTH, VIEWPORT_HEIGHT);
		pViewport->SetSizeStyle(Widgets::Widget::HSIZE_STRETCH | Widgets::Widget::VSIZE_STRETCH);
		pViewport->OnUpdate([this](uint64_t dt) { Viewport_OnUpdate(); });
		pViewport->OnRender([this]() { Viewport_OnRender(); });
		pViewport->OnPreRender([this]() { Viewport_OnPreRender(); });
		pRightSideSplit->AddTopPanel(pViewport);

		//create property grid
		PropertyGridWidget* pPropertyGrid = new PropertyGridWidget();
		pRightSideSplit->AddBottomPanel(pPropertyGrid);

		m_pPopulator = new PropertyGridPopulator();
		m_pPopulator->Init(pPropertyGrid);

		//split the left panel to, top for the list of meshes, bottom for the logs and materials
		Widgets::SplitHorizontal* pLeftPanelSplit = new Widgets::SplitHorizontal();
		pLeftPanelSplit->SetSizeStyle(Widgets::Widget::HSIZE_STRETCH | Widgets::Widget::VSIZE_STRETCH);
		pSplit->AddLeftPanel(pLeftPanelSplit);

		{
			Widgets::TableView* pMeshTableView = new Widgets::TableView();
			pLeftPanelSplit->AddTopPanel(pMeshTableView);

			m_pMeshListModel = new MeshListModel();
			m_pMeshListModel->GetSelectionModel()->OnSelectionChanged([this](const std::vector<Widgets::SelectionRow>& selected, const std::vector<Widgets::SelectionRow>& deselected)
				{
					MeshTableView_OnSelectionChanged(selected, deselected);
				});

			pMeshTableView->SetModel(m_pMeshListModel);
			pMeshTableView->SetColumnWidth(MeshListModel::Columns::Name, 250);
		}

		//bottom split : tab container for materials and logs
		Widgets::TabContainer* pTabContainer = new Widgets::TabContainer();
		pTabContainer->SetSizeStyle(Widgets::Widget::HSIZE_STRETCH | Widgets::Widget::VSIZE_STRETCH);
		pLeftPanelSplit->AddBottomPanel(pTabContainer);

		Widgets::Tab* pMaterialTab = new Widgets::Tab();
		pTabContainer->AddTab("Material", pMaterialTab);

		Widgets::Layout* pMaterialLayout = new Widgets::Layout(0, 0, 0, 0);
		pMaterialLayout->SetSizeStyle(Widgets::Widget::HSIZE_STRETCH | Widgets::Widget::VSIZE_STRETCH);
		pMaterialLayout->SetDirection(Widgets::Layout::Vertical);
		pMaterialTab->AddWidget(pMaterialLayout);

		pTabContainer->SetSelectedTab(0);

		//load all material
		Systems::AssetMgr& assetMgr = Systems::AssetMgr::Get();

		assetMgr.ForEachMetadata([this, pMaterialLayout](const Systems::AssetMetadata& metadata)
			{
				if (metadata.GetAssetType() != MAKESID("Material"))
					return;

				//create entry
				int materialIndex = static_cast<int>(m_allMaterials.size());
				m_allMaterials.push_back(MaterialEntry());
				MaterialEntry& materialEntry = m_allMaterials.back();
				materialEntry.m_Id = metadata.GetAssetId();
				Systems::AssetUtil::LoadAsset(materialEntry.m_Id);

				//material widget
				const int LINE_HEIGHT = 20;
				Widgets::Button* pButton = new Widgets::Button(0, LINE_HEIGHT, 0, 0);
				pButton->SetSizeStyle(Widgets::Widget::HSIZE_STRETCH | Widgets::Widget::VSIZE_DEFAULT);
				pButton->OnClick([this, materialIndex]() -> bool { return OnMaterialClicked(materialIndex); });
				pMaterialLayout->AddWidget(pButton);

				const int LABEL_OFFSET_X = 10;
				Widgets::Label* pLabel = new Widgets::Label(LABEL_OFFSET_X, 0, 1, metadata.GetVirtualName());
				pButton->AddWidget(pLabel);
			});

		//by default use the first material
		if(!m_allMaterials.empty())
			m_materialId = m_allMaterials.front().m_Id;

		//world axis render target
		{
			const int WIDTH = 1080;
			const int HEIGHT = 789;
			m_pWorldAxisRenderTarget = new Rendering::RenderTarget(WIDTH, HEIGHT); //make it smaller later
			m_pWorldAxisRTRatio = WIDTH / HEIGHT;
		}

		ComputeCameraPositionAndView();

		MeshEditorModule& meshModule = MeshEditorModule::Get();
		meshModule.OnMeshCreated([this](const Systems::AssetMetadata& metadata) { m_pMeshListModel->AddRow(metadata); });
		meshModule.OnMeshRenamed([this](const Systems::AssetMetadata& metadata) { m_pMeshListModel->OnMeshRenamed(metadata); });
		meshModule.OnBeforeMeshDeleted([this](const Systems::AssetMetadata& metadata) { m_pMeshListModel->RemoveRow(metadata.GetAssetId()); });
	}

	bool MeshEditor::OnMaterialClicked(int entryIndex)
	{
		const MaterialEntry& materialEntry = m_allMaterials[entryIndex];
		m_materialId = materialEntry.m_Id;

		return true;
	}

	void MeshEditor::OnClicked_File_Save()
	{
		if (!m_pSelectedMesh)
			return;

		bool res = MeshEditorModule::Get().SaveMesh(m_pSelectedMesh->GetId());

		const Systems::AssetMetadata* pMetadata = Systems::AssetMgr::Get().GetMetadata(m_pSelectedMesh->GetId());
		if (!pMetadata)
		{
			Core::LogModule::Get().LogError("Failed to find metadata for asset %s", m_pSelectedMesh->GetId().ToString().c_str());
			return;
		}

		if(res)
			Core::LogModule::Get().LogInfo("Mesh %s saved.", pMetadata->GetVirtualName().c_str());
		else
			Core::LogModule::Get().LogError("Failed to save mesh %s.", pMetadata->GetVirtualName().c_str());
	}

	void MeshEditor::OnClicked_File_Import()
	{
		std::string filename;
		bool res = Os::OpenFileDialog(filename);
		if (!res)
			return;

		MeshEditorModule::Get().ImportMesh(filename);
	}

	void MeshEditor::OnClicked_File_Reimport()
	{
		const Widgets::SelectionModel* pSelectionModel = m_pMeshListModel->GetSelectionModel();
		if (!pSelectionModel)
			return;

		const std::list<Widgets::SelectionRow>& rows = pSelectionModel->GetSelectedRows();
		if (rows.empty())
		{
			Core::LogModule::Get().LogError("Can't reimport mesh because there is no selection.");
			return;
		}
		
		MeshEditorModule& module = MeshEditorModule::Get();
		Systems::AssetMgr& assetMgr = Systems::AssetMgr::Get();

		for (const Widgets::SelectionRow& row : rows)
		{
			Systems::NewAssetId id = m_pMeshListModel->GetAssetId(row.GetStartIndex());
			if (!id.IsValid())
				continue;

			const Systems::AssetMetadata* pMetadata = assetMgr.GetMetadata(id);
			if (!pMetadata)
			{
				Core::LogModule::Get().LogError("Failed to find metadata for asset %s", m_pSelectedMesh->GetId().ToString().c_str());
				continue;
			}

			bool importResult = module.ReimportMesh(id);
			if (!importResult)
				Core::LogModule::Get().LogError("Failed to reimport %s", pMetadata->GetVirtualName().c_str());
			else
				Core::LogModule::Get().LogInfo("Mesh %s reimported.", pMetadata->GetVirtualName().c_str());
		}
	}

	void MeshEditor::OnClicked_File_Rename()
	{
		if (!m_pSelectedMesh)
			return;

		Systems::NewAssetId selectedMeshId = m_pSelectedMesh->GetId();
		Systems::AssetMetadata* pMetadata = Systems::AssetMgr::Get().GetMetadata(selectedMeshId);
		if (!pMetadata)
			return;

		const int BUFFER_SIZE = 256;
		char buffer[BUFFER_SIZE] = { '\0' };
		snprintf(buffer, BUFFER_SIZE, "Rename texture %s", pMetadata->GetVirtualName().c_str());

		UserInputDialog* pDialog = new UserInputDialog(buffer);
		pDialog->OnInputValidated([selectedMeshId](const std::string& input)
			{
				MeshEditorModule::Get().RenameMesh(selectedMeshId, input);
			});
		pDialog->Open();
	}

	void MeshEditor::OnClicked_File_Delete()
	{
		if (!m_pSelectedMesh)
			return;

		Systems::NewAssetId selectedAssetId = m_pSelectedMesh->GetId();

		Systems::AssetMetadata* pMetadata = Systems::AssetMgr::Get().GetMetadata(selectedAssetId);
		if (!pMetadata)
			return;

		const int BUFFER_SIZE = 256;
		char buffer[BUFFER_SIZE] = { '\0' };
		snprintf(buffer, BUFFER_SIZE, "Are you sure you want to delete the mesh %s?", pMetadata->GetVirtualName().c_str());

		OkCancelDialog* pDialog = new OkCancelDialog("Delete", buffer);
		pDialog->OnOk([selectedAssetId]() { MeshEditorModule::Get().DeleteMesh(selectedAssetId); });
		pDialog->Open();
	}

	void MeshEditor::Viewport_OnUpdate()
	{
		Inputs::InputMgr& inputs = Inputs::InputMgr::Get();
		if (inputs.IsMouseLeftButtonDown())
		{
			Core::Int2 mousePosition = inputs.GetMousePosition();
			if (m_firstFrameMouseDown)
			{
				m_mousePreviousPos = mousePosition;
				m_firstFrameMouseDown = false;
			}

			Core::Int2 delta;
			delta.x = m_mousePreviousPos.x - mousePosition.x;
			delta.y = m_mousePreviousPos.y - mousePosition.y;

			const float ROTATION_SPEED = 0.01f;
			Core::Vec4f offset(static_cast<float>(delta.y) * ROTATION_SPEED, -static_cast<float>(delta.x) * ROTATION_SPEED, 0, 0);

			m_cameraEuler = m_cameraEuler + offset;

			m_mousePreviousPos = mousePosition;
		}
		else if (!m_firstFrameMouseDown)
		{
			m_firstFrameMouseDown = true;
		}

		int16_t mouseWheelDistance = inputs.GetMouseWheelDistance();
		const float CAMERA_DISTANCE_SPEED = 0.05f;
		const float MIN_DISTANCE = 2;
		if (mouseWheelDistance != 0)
			m_cameraDistance -= mouseWheelDistance * CAMERA_DISTANCE_SPEED;

		if (m_cameraDistance < MIN_DISTANCE)
			m_cameraDistance = MIN_DISTANCE;

		ComputeCameraPositionAndView();
	}

	void MeshEditor::Viewport_OnRender()
	{
		//world
		Core::Mat44f world = Core::Mat44f::CreateIdentity();

		//projection
		constexpr float fov = 45.f;
		constexpr float fovRad = fov * Core::PI_OVER_180;
		float nearDistance = 0.1f;

		Core::Mat44f proj = Core::Mat44f::CreatePerspective(fovRad, m_aspectRatio, nearDistance, 1000.f);

		if (m_pSelectedMesh)
		{
			Core::Mat44f mvpMatrix = world * m_cameraView * proj;

			Rendering::RenderModule& renderer = Rendering::RenderModule::Get();

			Systems::MaterialAsset* pMaterial = Systems::AssetUtil::GetAsset<Systems::MaterialAsset>(m_materialId);
			if (pMaterial && pMaterial->IsValidForRendering())
			{
				Rendering::PerObjectCBuffer perObjectData;
				perObjectData.m_world = Core::Mat44f(world);

				Core::Float3 cameraPosFloat3(m_cameraPosition.GetX(), m_cameraPosition.GetY(), m_cameraPosition.GetZ());
				Rendering::PerFrameCBuffer perFrameData(m_cameraView, proj, cameraPosFloat3);

				Rendering::LightsArrayCBuffer lights;
				lights.AddLight()->MakeDirectionalLight(Core::Float3(0, -1, 0), Core::Float3(1, 1, 1), Core::Float3(1, 1, 1), Core::Float3(1, 1, 1));

				Systems::MaterialRendering::Bind(*pMaterial, perObjectData, perFrameData, lights);

				const Rendering::Mesh* pMesh = m_pSelectedMesh->GetRenderingMesh();
				renderer.RenderMesh(*pMesh);
			}
		}

	}

	void MeshEditor::Viewport_OnPreRender()
	{
		Rendering::RenderModule& renderer = Rendering::RenderModule::Get();
		renderer.GetCamera()->SetLookAt(m_cameraPosition, m_cameraTarget, Core::Vec4f(0, 1, 0, 0));
		renderer.GetCamera()->SetProjection(45 * Core::PI_OVER_180, m_pWorldAxisRTRatio, 0.1f, 1000);

		m_pWorldAxisRenderTarget->BeginScene();

		DirectX::XMMATRIX identity = DirectX::XMMatrixIdentity();
		renderer.RenderPrimitiveCube(identity, Core::Float4(1, 0, 0, 1));

		m_pWorldAxisRenderTarget->EndScene();

		m_pWorldAxisRenderTarget->GetColorTexture()->TransitionToShaderResource();
	}

	void MeshEditor::MeshTableView_OnSelectionChanged(const std::vector<Widgets::SelectionRow>& selected, const std::vector<Widgets::SelectionRow>& deselected)
	{
		if (selected.empty())
		{
			m_pPopulator->Populate(nullptr);
			m_pSelectedMesh = nullptr;
			return;
		}

		const Widgets::SelectionRow& selectedRow = selected[0];
		Systems::NewAssetId meshId = m_pMeshListModel->GetAssetId(selectedRow.GetStartIndex());

		m_pSelectedMesh = Systems::AssetUtil::LoadAsset<Systems::MeshAsset>(meshId);

		m_pPopulator->Populate(m_pSelectedMesh);
	}

	void MeshEditor::ComputeCameraPositionAndView()
	{
		//view
		Core::Vec4f cameraUp(0, 1, 0, 1);
		Core::Vec4f cameraLookAt(0, 0, 0, 1);

		//calculate the camera position
		Core::Mat44f rotY = Core::Mat44f::CreateRotationY(m_cameraEuler.GetY());
		Core::Mat44f rotX = Core::Mat44f::CreateRotationX(m_cameraEuler.GetX());
		Core::Mat44f orientation = rotX * rotY;

		Core::Mat44f tx = Core::Mat44f::CreateTranslationMatrix(Core::Vec4f(0, 0, m_cameraDistance, 1));

		Core::Mat44f txPos = tx * orientation;
		m_cameraPosition = m_cameraTarget * txPos;

		Core::Vec4f cameraDirection = cameraLookAt - m_cameraPosition;

		m_cameraView = Core::Mat44f::CreateView(m_cameraPosition, cameraDirection, cameraUp);
	}
}
