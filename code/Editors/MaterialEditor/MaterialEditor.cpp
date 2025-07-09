/********************************************************************/
/* � 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/MaterialEditor/MaterialEditor.h"

#include "Core/Log/LogModule.h"

#include "Editors/MaterialEditor/MaterialEditorModule.h"
#include "Editors/MaterialEditor/MaterialListModel.h"
#include "Editors/MaterialEditor/ShaderTemplate.h"
#include "Editors/Widgets/Dialog/OkCancelDialog.h"
#include "Editors/Widgets/Dialog/UserInputDialog.h"
#include "Editors/Widgets/PropertyGrid/ItemFactory/PropertyGridItemFactory_MaterialParameterDescription.h"
#include "Editors/Widgets/PropertyGrid/PropertyGridPopulator.h"
#include "Editors/Widgets/PropertyGrid/PropertyGridWidget.h"
#include "Editors/Widgets/StatusBar/StatusBar.h"

#include "Inputs/InputMgr.h"

#include "OsWin/Process.h"
#include "OsWin/FileDialog.h"

#include "Rendering/ConstantBuffer/LightsCBuffer.h"
#include "Rendering/ConstantBuffer/LinearConstantBufferPool.h"
#include "Rendering/ConstantBuffer/PerFrameCBuffer.h"
#include "Rendering/ConstantBuffer/PerObjectCBuffer.h"
#include "Rendering/RenderModule.h"

#include "Systems/Assets/AssetMgr.h"
#include "Systems/Assets/AssetObjects/AssetUtil.h"
#include "Systems/Assets/AssetObjects/MeshAsset.h"
#include "Systems/Container/Container.h"
#include "Systems/Container/ContainerMgr.h"
#include "Systems/Objects/AssetObject.h"
#include "Systems/Rendering/MaterialRendering.h"

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
		, m_pMaterialListModel(nullptr)
		, m_selectedMaterialId(Systems::NewAssetId::INVALID)
		, m_pPropertyGridPopulator(new PropertyGridPopulator())
		, m_cameraDistance(10)
		, m_aspectRatio(0.f)
		, m_pMesh(nullptr)
		, m_firstFrameMouseDown(true)
		, m_mousePreviousPos()
	{
		m_cameraEuler = DirectX::XMVectorSet(0, 0, 0, 1);
		m_cameraTarget = DirectX::XMVectorSet(0, 0, 0, 1);
	}

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

		Widgets::Layout* pOutsideLayout = new Widgets::Layout();
		pOutsideLayout->SetDirection(Widgets::Layout::Vertical_Reverse);
		pOutsideLayout->SetSizeStyle(Widgets::Widget::STRETCH);
		pViewportTab->AddWidget(pOutsideLayout);

		StatusBar* pStatusBar = new StatusBar();
		pOutsideLayout->AddWidget(pStatusBar);

		Widgets::Layout* pInternalLayout = new Widgets::Layout();
		pInternalLayout->SetDirection(Widgets::Layout::Direction::Vertical);
		pInternalLayout->SetSizeStyle(Widgets::Widget::SIZE_STYLE::STRETCH);
		pOutsideLayout->AddWidget(pInternalLayout);

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
			const int WIDTH = 1080;
			const int HEIGHT = 789;
			m_aspectRatio = WIDTH / static_cast<float>(HEIGHT);

			Widgets::Viewport_v2* pViewport = new Widgets::Viewport_v2(WIDTH, HEIGHT);
			pViewport->SetSizeStyle(Widgets::Widget::HSIZE_STRETCH | Widgets::Widget::VSIZE_STRETCH);
			pViewport->OnRender([this]() { Viewport_OnRender(); });
			pViewport->OnUpdate([this](uint64_t dt) { Viewport_OnUpdate(dt); });
			pHSplit->AddTopPanel(pViewport);
		}

		//right panel : add option layout
		Widgets::Layout* pRightPanelLayout = new Widgets::Layout(0, 0, 0, 0);
		pRightPanelLayout->SetSizeStyle(Widgets::Widget::HSIZE_STRETCH | Widgets::Widget::VSIZE_STRETCH);
		pRightPanelLayout->SetDirection(Widgets::Layout::Direction::Vertical);
		pHSplit->AddBottomPanel(pRightPanelLayout);
		
		//horizontal layout for buttons
		Widgets::Layout* pButtonLayout = new Widgets::Layout(0, 20, 0, 0);
		pButtonLayout->SetSizeStyle(Widgets::Widget::HSIZE_STRETCH | Widgets::Widget::VSIZE_FIT);
		pButtonLayout->SetDirection(Widgets::Layout::Direction::Horizontal);
		pRightPanelLayout->AddWidget(pButtonLayout);

		{
			Widgets::Button* pButton = new Widgets::Button("New HLSL");
			pButton->OnClick([this]() -> bool { return OnNewHlslClicked(); });
			pButtonLayout->AddWidget(pButton);
		}

		//add compile button
		{
			Widgets::Button* pButton = new Widgets::Button("Compile");
			pButton->OnClick([this]() -> bool { return OnCompileClicked(); });
			pButtonLayout->AddWidget(pButton);
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

		MaterialEditorModule::Get().OnMaterialCreated([this](const Systems::AssetMetadata* pMetadata)
			{
				m_pMaterialListModel->AddRow(pMetadata);
			});

		MaterialEditorModule::Get().OnMaterialInstanceCreated([this](const Systems::AssetMetadata* pMetadata)
			{
				m_pMaterialListModel->AddRow(pMetadata);
				m_pMaterialListModel->SetShaderModified(pMetadata->GetAssetId());
			});

		MaterialEditorModule::Get().OnMaterialRenamed([this](const Systems::AssetMetadata& metadata, const std::string& oldName)
			{
				m_pMaterialListModel->OnMaterialRenamed(metadata);
			});


		//find the cube to render in the viewport
		Systems::AssetMetadata metadataCube;
		Systems::AssetMgr::Get().ForEachMetadata([&metadataCube](const Systems::AssetMetadata& metadata)
			{
				if (metadata.GetAssetType() != CONSTSID("Mesh"))
					return;

				std::string meshName = "base_sphere.fbx";
				if (metadata.GetVirtualName().compare(metadata.GetVirtualName().size() - meshName.size(), meshName.size(), meshName) == 0)
					metadataCube = metadata;
			});

		m_pMesh = Systems::AssetUtil::LoadAsset<Systems::MeshAsset>(metadataCube.GetAssetId());
	}

	void MaterialEditor::CreateMenu(Widgets::Widget* pParent)
	{
		Widgets::MenuBar* pMenuBar = new Widgets::MenuBar();
		pParent->AddWidget(pMenuBar);

		//create the file menu
		{
			Widgets::Menu* pFileMenu = pMenuBar->AddMenu("File");

			Widgets::MenuItem* pNewItem = pFileMenu->AddMenuItem("New Material...");
			pNewItem->SetShortcut("Ctrl+N");
			pNewItem->OnClick([this]() { MenuFile_NewMaterial_OnClicked(); });

			Widgets::MenuItem* pNewInstanceItem = pFileMenu->AddMenuItem("New Material Instance...");
			pNewInstanceItem->OnClick([this]() { MenuFile_NewMaterialInstance_OnClicked(); });

			Widgets::MenuItem* pSaveItem = pFileMenu->AddMenuItem("Save Material");
			pSaveItem->SetShortcut("Ctrl+S");
			pSaveItem->OnClick([this]() { MenuFile_Save_OnClicked(); });

			Widgets::MenuItem* pRenameItem = pFileMenu->AddMenuItem("Rename Material");
			pRenameItem->SetShortcut("F2");
			pRenameItem->OnClick([this]() { MenuFile_Rename_OnClicked(); });

			Widgets::MenuItem* pDeleteItem = pFileMenu->AddMenuItem("Delete Material");
			pDeleteItem->SetShortcut("Del");
			pDeleteItem->OnClick([this]() { MenuFile_Delete_OnClicked(); });
		}
	}

	void MaterialEditor::MenuFile_NewMaterial_OnClicked()
	{
		//modal windows are automatically deleted when closed,so no need to delete the dialog.
		UserInputDialog* pDialog = new UserInputDialog("New Asset Name");
		pDialog->OnInputValidated([this](const std::string& input) 
			{ 
				Systems::MaterialAsset* pMaterial = MaterialEditorModule::Get().NewMaterial(input);

				Widgets::SelectionModel* pSelectionModel = m_pMaterialListModel->GetSelectionModel();

				int columnCount = m_pMaterialListModel->GetColumnCount(Widgets::ModelIndex());
				Widgets::ModelIndex start = m_pMaterialListModel->GetIndex(pMaterial->GetId());
				Widgets::ModelIndex end = start.GetSiblingAtColumn(columnCount - 1);
				Widgets::SelectionRow selection(start, end);

				pSelectionModel->SetSelectionRow(selection);
			});
		pDialog->Open();
	}

	void MaterialEditor::MenuFile_NewMaterialInstance_OnClicked()
	{
		//check first if the currently selected material is a base material
		const Systems::AssetMetadata* pBaseMetadata = Systems::AssetMgr::Get().GetMetadata(m_selectedMaterialId);
		if (!pBaseMetadata)
			return;

		if (pBaseMetadata->GetAssetType() != Systems::MaterialAsset::GetAssetTypeNameSid())
		{
			Core::LogModule::Get().LogError("The selected material is not a base material.");
			return;
		}

		UserInputDialog* pDialog = new UserInputDialog("New material instance name");
		pDialog->OnInputValidated([this](const std::string& input)
			{
				Systems::MaterialInstanceAsset* pMaterial = MaterialEditorModule::Get().NewMaterialInstance(input, m_selectedMaterialId);

				Widgets::SelectionModel* pSelectionModel = m_pMaterialListModel->GetSelectionModel();

				int columnCount = m_pMaterialListModel->GetColumnCount(Widgets::ModelIndex());
				Widgets::ModelIndex start = m_pMaterialListModel->GetIndex(pMaterial->GetId());
				Widgets::ModelIndex end = start.GetSiblingAtColumn(columnCount - 1);
				Widgets::SelectionRow selection(start, end);

				pSelectionModel->SetSelectionRow(selection);
			});
		pDialog->Open();
	}

	void MaterialEditor::MenuFile_Save_OnClicked()
	{
		Widgets::SelectionModel* pSelectionModel = m_pMaterialListModel->GetSelectionModel();
		const std::list<Widgets::SelectionRow>& selection = pSelectionModel->GetSelectedRows();
		if (selection.empty())
			return;

		const Widgets::SelectionRow& row = selection.front();
		Systems::NewAssetId id = m_pMaterialListModel->GetAssetId(row.GetStartIndex());
		MaterialEditorModule::Get().SaveMaterial(id);

		m_pMaterialListModel->ClearShaderModified(id);
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

	void MaterialEditor::MenuFile_Rename_OnClicked()
	{
		if (!m_selectedMaterialId.IsValid())
		{
			Core::LogModule::Get().LogError("You need to select a material to rename.");
			return;
		}

		UserInputDialog* pDialog = new UserInputDialog("New name");
		pDialog->OnInputValidated([this](const std::string& input)
			{
				MaterialEditorModule::Get().RenameMaterial(m_selectedMaterialId, input);
			});
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
		Widgets::SelectionModel* pSelectionModel = m_pMaterialListModel->GetSelectionModel();
		const std::list<Widgets::SelectionRow>& selection = pSelectionModel->GetSelectedRows();
		if (selection.empty())
			return true;

		const Widgets::SelectionRow& row = selection.front();
		Systems::NewAssetId id = m_pMaterialListModel->GetAssetId(row.GetStartIndex());

		bool res = MaterialEditorModule::Get().CompileMaterial(id);

		RefreshPropertyGrid();

		if (!res)
			return true;

		m_pMaterialListModel->SetShaderModified(id);

		return true;
	}

	bool MaterialEditor::OnNewHlslClicked()
	{
		std::string folder;
		bool res = Os::SelectFolderDialog("Select folder where to store the hlsl files", folder);
		if (!res)
			return true;

		std::string vertexFile = folder + "\\vs.hlsl";
		ShaderTemplate::WriteVertexShaderTemplate(vertexFile);

		std::string pixelFile = folder + "\\ps.hlsl";
		ShaderTemplate::WritePixelShaderTemplate(pixelFile);

		const Systems::AssetMetadata* pMetadata = Systems::AssetMgr::Get().GetMetadata(m_selectedMaterialId);
		if (!pMetadata)
			return true;

		if (!pMetadata->IsA<Systems::MaterialAsset>())
			return true;

		Systems::MaterialAsset* pMaterial = Systems::AssetUtil::LoadAsset<Systems::MaterialAsset>(m_selectedMaterialId);
		if (!pMaterial)
			return false;

		pMaterial->SetSourceFileVs(vertexFile);
		pMaterial->SetSourceFilePs(pixelFile);

		return true;
	}

	void MaterialEditor::PropertyGridPopulator_OnDataChanged()
	{
		m_pMaterialListModel->SetShaderModified(m_selectedMaterialId);
	}

	void MaterialEditor::DeleteSelectedShader()
	{
		if (!m_selectedMaterialId.IsValid())
			return;

		//removing the row will change m_selectedMaterialId, so store it.
		Systems::NewAssetId materialToDelete = m_selectedMaterialId;

		//first delete it from the model cause the model can need the material
		m_pMaterialListModel->RemoveRow(materialToDelete);

		//then delete it for real
		MaterialEditorModule::Get().DeleteMaterial(materialToDelete);

		
	}

	void MaterialEditor::CreateShadersList()
	{
		Widgets::TableView* pTableView = new Widgets::TableView();
		pTableView->SetSizeStyle(Widgets::Widget::STRETCH);
		m_pShaderListLayout->AddWidget(pTableView);

		m_pMaterialListModel = new MaterialListModel();
		pTableView->SetModel(m_pMaterialListModel);
		pTableView->SetColumnWidth(MaterialListModel::Columns::Name, 200);

		Widgets::SelectionModel* pSelectionModel = m_pMaterialListModel->GetSelectionModel();
		pSelectionModel->OnSelectionChanged([this](const std::vector<Widgets::SelectionRow>& selected, const std::vector<Widgets::SelectionRow>& deselected)
			{
				if (selected.size() == 0)
				{
					m_selectedMaterialId = Systems::NewAssetId::INVALID;
					m_pPropertyGrid->ClearAllItems();
					return;
				}

				const Widgets::SelectionRow& row = selected[0];
				Systems::NewAssetId id = m_pMaterialListModel->GetAssetId(row.GetStartIndex());
				if (m_selectedMaterialId == id)
					return;

				m_selectedMaterialId = id;
				OnShaderEntryClicked(id);
			});
	}

	void MaterialEditor::Viewport_OnRender()
	{
		if (!m_pMesh)
			return;

		if (m_selectedMaterialId == Systems::NewAssetId::INVALID)
			return;

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
		DirectX::XMMATRIX mvpMatrix = DirectX::XMMatrixMultiply(world, view);
		mvpMatrix = DirectX::XMMatrixMultiply(mvpMatrix, projection);

		Rendering::RenderModule& renderer = Rendering::RenderModule::Get();

		bool isBaseMaterial = Systems::AssetUtil::IsA<Systems::MaterialAsset>(m_selectedMaterialId);
		if (isBaseMaterial)
		{
			Systems::MaterialAsset* pMaterial = Systems::AssetUtil::GetAsset<Systems::MaterialAsset>(m_selectedMaterialId);
			if (pMaterial && pMaterial->IsValidForRendering())
			{
				Rendering::PerObjectCBuffer perObjectData(world);

				DirectX::XMFLOAT3 cameraPosFloat3;
				DirectX::XMStoreFloat3(&cameraPosFloat3, cameraPosition);
				Rendering::PerFrameCBuffer perFrameData(view, projection, cameraPosFloat3);

				Rendering::Light dirLight = Rendering::Light::MakeDirectionalLight(DirectX::XMFLOAT3(0, -1, 0));
				Rendering::LightsCBuffer lights(dirLight);

				Systems::MaterialRendering::Bind(*pMaterial, perObjectData, perFrameData, lights);

				const Rendering::Mesh* pMesh = m_pMesh->GetRenderingMesh();
				renderer.RenderMesh(*pMesh);
			}
		}
		else if (Systems::AssetUtil::IsA<Systems::MaterialInstanceAsset>(m_selectedMaterialId))
		{
			const Systems::MaterialInstanceAsset* pMaterialInstance = Systems::AssetUtil::GetAsset<Systems::MaterialInstanceAsset>(m_selectedMaterialId);
			const Systems::MaterialAsset* pMaterial = pMaterialInstance->GetBaseMaterial();
			if (pMaterial && pMaterial->IsValidForRendering())
			{
				Rendering::PerObjectCBuffer perObjectData(world);

				DirectX::XMFLOAT3 cameraPosFloat3;
				DirectX::XMStoreFloat3(&cameraPosFloat3, cameraPosition);
				Rendering::PerFrameCBuffer perFrameData(view, projection, cameraPosFloat3);

				Rendering::Light dirLight = Rendering::Light::MakeDirectionalLight(DirectX::XMFLOAT3(0, -1, 0));
				Rendering::LightsCBuffer lights(dirLight);

				Systems::MaterialRendering::Bind(*pMaterialInstance, perObjectData, perFrameData, lights);

				const Rendering::Mesh* pMesh = m_pMesh->GetRenderingMesh();
				renderer.RenderMesh(*pMesh);
			}
		}
	}

	void MaterialEditor::Viewport_OnUpdate(uint64_t dt)
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

	void MaterialEditor::RefreshPropertyGrid()
	{
		m_pPropertyGrid->ClearAllItems();

		if (m_selectedMaterialId == Systems::NewAssetId::INVALID)
			return;

		Systems::ContainerMgr& containerMgr = Systems::ContainerMgr::Get();
		Systems::Container* pContainer = containerMgr.GetContainer(m_selectedMaterialId.GetContainerId());
		if (!pContainer)
		{
			pContainer = containerMgr.LoadContainer(m_selectedMaterialId.GetContainerId());
		}

		if (!pContainer)
			return;

		Systems::AssetObject* pObject = pContainer->GetAsset(m_selectedMaterialId.GetObjectId());
		if (!pObject)
			return;

		m_pPropertyGridPopulator->Populate(pObject);
	}
}
