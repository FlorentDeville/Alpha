/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/MeshEditor/MeshEditor.h"

#include "Editors/MeshEditor/MeshEditorModule.h"
#include "Editors/MeshEditor/MeshListModel.h"
#include "Editors/Widgets/Dialog/OkCancelDialog.h"
#include "Editors/Widgets/Dialog/UserInputDialog.h"

#include "Inputs/InputMgr.h"

#include "OsWin/FileDialog.h"

#include "Rendering/ConstantBuffer/LightsCBuffer.h"
#include "Rendering/ConstantBuffer/PerFrameCBuffer.h"
#include "Rendering/ConstantBuffer/PerObjectCBuffer.h"
#include "Rendering/RenderModule.h"

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
		, m_pLogWidget(nullptr)
		, m_allMaterials()
		, m_pSelectedMesh(nullptr)
		, m_pMeshListModel(nullptr)
	{
		m_cameraEuler = DirectX::XMVectorSet(0, 0, 0, 1);
		m_cameraTarget = DirectX::XMVectorSet(0, 0, 0, 1);
	}

	MeshEditor::~MeshEditor()
	{ }

	void MeshEditor::CreateEditor(Widgets::Widget* pParent)
	{
		CreateDefaultWidgets(pParent, "Mesh");

		//create the file menu
		{
			Widgets::Menu* pFileMenu = m_pMenuBar->AddMenu("File");

			Widgets::MenuItem* pNewItem = pFileMenu->AddMenuItem("Import...");
			pNewItem->OnClick([this]() { OnClicked_File_Import(); });

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

		//create left viewport
		const int VIEWPORT_WIDTH = 1280;
		const int VIEWPORT_HEIGHT = 720;
		m_aspectRatio = VIEWPORT_WIDTH / static_cast<float>(VIEWPORT_HEIGHT);
		Widgets::Viewport* pViewport = new Widgets::Viewport(VIEWPORT_WIDTH, VIEWPORT_HEIGHT);
		pViewport->SetSizeStyle(Widgets::Widget::HSIZE_STRETCH | Widgets::Widget::VSIZE_STRETCH);
		pViewport->OnUpdate([this](uint64_t dt) { Viewport_OnUpdate(); });
		pViewport->OnRender([this]() { Viewport_OnRender(); });
		pSplit->AddRightPanel(pViewport);

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

		Widgets::Tab* pLogTab = new Widgets::Tab();
		pTabContainer->AddTab("Log", pLogTab);

		m_pLogWidget = new Widgets::Text(1, "");
		m_pLogWidget->SetSizeStyle(Widgets::Widget::HSIZE_STRETCH | Widgets::Widget::VSIZE_STRETCH);
		pLogTab->AddWidget(m_pLogWidget);

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

		MeshEditorModule& meshModule = MeshEditorModule::Get();
		meshModule.OnMeshCreated([this](const Systems::AssetMetadata& metadata) { m_pMeshListModel->AddRow(metadata); });
		meshModule.OnMeshRenamed([this](const Systems::AssetMetadata& metadata) { m_pMeshListModel->OnMeshRenamed(metadata); });
		meshModule.OnBeforeMeshDeleted([this](const Systems::AssetMetadata& metadata) { m_pMeshListModel->RemoveRow(metadata.GetAssetId()); });
	}

	bool MeshEditor::OnMeshImportClicked(int entryIndex)
	{
		/*MeshEntry& entry = m_allMeshes[entryIndex];
		if (!entry.m_mesh)
			LoadMesh(entry);

		if (!entry.m_mesh)
		{
			m_pLogWidget->AppendText("Failed to load asset.");
			return true;
		}

		FbxImporter::FbxImporter importer;
		bool res = importer.Import(entry.m_mesh->GetSourceFile(), *entry.m_mesh);
		if (!res)
		{
			m_pLogWidget->AppendText("Failed to import asset.");
			return true;
		}

		return true;*/
		return true;
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

		MeshEditorModule::Get().SaveMesh(m_pSelectedMesh->GetId());
	}

	void MeshEditor::OnClicked_File_Import()
	{
		std::string filename;
		bool res = Os::OpenFileDialog(filename);
		if (!res)
			return;

		MeshEditorModule::Get().ImportMesh(filename);
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
			DirectX::XMUINT2 mousePosition;
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
			DirectX::XMVECTOR offset = DirectX::XMVectorSet(static_cast<float>(delta.y) * ROTATION_SPEED, -static_cast<float>(delta.x) * ROTATION_SPEED, 0, 0);

			m_cameraEuler = DirectX::XMVectorAdd(m_cameraEuler, offset);

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
	}

	void MeshEditor::Viewport_OnRender()
	{
		//world
		DirectX::XMMATRIX world = DirectX::XMMatrixIdentity();

		//view
		DirectX::XMVECTOR cameraUp = DirectX::XMVectorSet(0, 1, 0, 1);
		DirectX::XMVECTOR cameraLookAt = DirectX::XMVectorSet(0, 0, 0, 1);

		//calculate the camera position
		DirectX::XMMATRIX orientation = DirectX::XMMatrixRotationRollPitchYawFromVector(m_cameraEuler);
		DirectX::XMMATRIX tx = DirectX::XMMatrixTranslationFromVector(DirectX::XMVectorSet(0, 0, m_cameraDistance, 1));
		DirectX::XMMATRIX txPos = DirectX::XMMatrixMultiply(tx, orientation);
		DirectX::XMVECTOR cameraPosition = DirectX::XMVector3Transform(m_cameraTarget, txPos);


		DirectX::XMVECTOR cameraDirection = DirectX::XMVectorSubtract(cameraLookAt, cameraPosition);
		cameraDirection = DirectX::XMVector4Normalize(cameraDirection);

		DirectX::XMMATRIX view = DirectX::XMMatrixLookToLH(cameraPosition, cameraDirection, cameraUp);

		//projection
		const float fov = 45.f;
		float nearDistance = 0.1f;
		float fovRad = DirectX::XMConvertToRadians(fov);
		DirectX::XMMATRIX projection = DirectX::XMMatrixPerspectiveFovLH(fovRad, m_aspectRatio, nearDistance, 100.0f);

		//RENDER
		if (m_pSelectedMesh)
		{
			DirectX::XMMATRIX mvpMatrix = DirectX::XMMatrixMultiply(world, view);
			mvpMatrix = DirectX::XMMatrixMultiply(mvpMatrix, projection);

			Rendering::RenderModule& renderer = Rendering::RenderModule::Get();

			Systems::MaterialAsset* pMaterial = Systems::AssetUtil::GetAsset<Systems::MaterialAsset>(m_materialId);
			if (pMaterial && pMaterial->IsValidForRendering())
			{
				Rendering::PerObjectCBuffer perObjectData;
				perObjectData.m_world = world;

				DirectX::XMFLOAT3 cameraPosFloat3;
				DirectX::XMStoreFloat3(&cameraPosFloat3, cameraPosition);
				Rendering::PerFrameCBuffer perFrameData(view, projection, cameraPosFloat3);

				Rendering::LightsArrayCBuffer lights;
				lights.AddLight()->MakeDirectionalLight(Core::Float3(0, -1, 0), Core::Float3(1, 1, 1), Core::Float3(1, 1, 1), Core::Float3(1, 1, 1));

				Systems::MaterialRendering::Bind(*pMaterial, perObjectData, perFrameData, lights);

				const Rendering::Mesh* pMesh = m_pSelectedMesh->GetRenderingMesh();
				renderer.RenderMesh(*pMesh);
			}
		}
	}

	void MeshEditor::MeshTableView_OnSelectionChanged(const std::vector<Widgets::SelectionRow>& selected, const std::vector<Widgets::SelectionRow>& deselected)
	{
		if (selected.empty())
		{
			m_pSelectedMesh = nullptr;
			return;
		}

		const Widgets::SelectionRow& selectedRow = selected[0];
		Systems::NewAssetId meshId = m_pMeshListModel->GetAssetId(selectedRow.GetStartIndex());

		m_pSelectedMesh = Systems::AssetUtil::LoadAsset<Systems::MeshAsset>(meshId);
	}
}
