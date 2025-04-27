/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/MeshEditor/MeshEditor.h"

#include "Importer/FbxImporter/FbxImporter.h"
#include "Inputs/InputMgr.h"

#include "OsWin/FileDialog.h"
#include "OsWin/Resource.h"

#include "Rendering/Material/Material.h"
#include "Rendering/Material/MaterialMgr.h"
#include "Rendering/Mesh/MeshMgr.h"
#include "Rendering/Mesh/Mesh.h"
#include "Rendering/PipelineState/PipelineStateMgr.h"
#include "Rendering/RenderModule.h"
#include "Rendering/RenderTargets/RenderTarget.h"
#include "Rendering/RootSignature/RootSignatureMgr.h"
#include "Rendering/Shaders/ShaderMgr.h"
#include "Rendering/Texture/TextureMgr.h"
#include "Rendering/Texture/Texture.h"

#include "Resources/ResourcesMgr.h"

#include "Systems/Assets/Asset.h"
#include "Systems/Assets/AssetMgr.h"
#include "Systems/Assets/AssetObjects/MeshAsset.h"
#include "Systems/Container/Container.h"
#include "Systems/Container/ContainerMgr.h"
#include "Systems/Loader.h"

#include "Widgets/Button.h"
#include "Widgets/Icon.h"
#include "Widgets/Menu.h"
#include "Widgets/MenuBar.h"
#include "Widgets/MenuItem.h"
#include "Widgets/Label.h"
#include "Widgets/Layout.h"
#include "Widgets/SplitHorizontal.h"
#include "Widgets/SplitVertical.h"
#include "Widgets/Tab.h"
#include "Widgets/TabContainer.h"
#include "Widgets/Text.h"
#include "Widgets/Viewport.h"

namespace Editors
{
	static void Imports()
	{
		std::string filename;
		bool res = Os::OpenFileDialog(filename);
		if (!res)
			return;

		Systems::ContainerMgr& containerMgr = Systems::ContainerMgr::Get();
		Systems::AssetMgr& assetMgr = Systems::AssetMgr::Get();

		Systems::MeshAsset* pMesh = Systems::CreateNewAsset<Systems::MeshAsset>();
		FbxImporter::FbxImporter importer;
		importer.Import(filename, *pMesh);
	
		Systems::Container* pContainer = containerMgr.CreateContainer(filename.c_str());
		pContainer->AddAsset(pMesh);

		Systems::AssetMetadata metadata(pMesh->GetId(), filename, MAKESID("Mesh"));
		assetMgr.RegisterAssetMetadata(metadata);

		containerMgr.SaveContainer(pContainer->GetId());

		assetMgr.SaveMetadataTable();
	}

	MeshEntry::MeshEntry()
		: m_displayName()
		, m_mesh()
	{}

	MaterialEntry::MaterialEntry()
		: m_name()
		, m_materialId()
	{}

	MeshEditor::MeshEditor()
		: Core::Singleton<MeshEditor>()
		, m_pRenderTarget(nullptr)
		, m_selectedMesh(-1)
		, m_cameraDistance(10.f)
		, m_materialId(Rendering::MaterialId::INVALID)
		, m_aspectRatio(0.f)
		, m_enableViewportControl(false)
		, m_firstFrameMouseDown(true)
		, m_mousePreviousPos(0, 0)
		, m_allEntryButton()
		, m_pLogWidget(nullptr)
		, m_allMaterials()
	{
		m_cameraEuler = DirectX::XMVectorSet(0, 0, 0, 1);
		m_cameraTarget = DirectX::XMVectorSet(0, 0, 0, 1);
	}

	MeshEditor::~MeshEditor()
	{
		delete m_pRenderTarget;
	}

	void MeshEditor::CreateEditor(const MeshEditorParameter& parameter)
	{
		m_blender = parameter.m_blender;
		m_editorScriptsPath = parameter.m_editorScriptsPath;

		Rendering::Texture* pImportIconTexture;
		Rendering::TextureMgr::Get().CreateTexture(&pImportIconTexture, m_importIconTextureId);

		const AppResources::ResourcesMgr& resourceMgr = AppResources::ResourcesMgr::Get();
		int16_t sysId = resourceMgr.GetSystemResourceId(AppResources::kUiIconImport);
		const char* type = resourceMgr.GetSystemResourceType(AppResources::kUiIconImport);
		char* pData;
		uint32_t dataSize;
		Os::Resource::GetResource(sysId, type, &pData, dataSize);
		pImportIconTexture->Init(pData, dataSize);

		//create the render target
		const int width = 1280;
		const int height = 720;
		m_aspectRatio = width / static_cast<float>(height);
		m_pRenderTarget = Rendering::RenderModule::Get().CreateRenderTarget(width, height);

		//create the widgets
		Widgets::Tab* pViewportTab = new Widgets::Tab();
		{
			Widgets::TabContainer* pTabContainer = dynamic_cast<Widgets::TabContainer*>(parameter.pParent);
			if (pTabContainer)
			{
				pTabContainer->AddTab("Mesh", pViewportTab);
			}
			else
			{
				parameter.pParent->AddWidget(pViewportTab);
			}
		}

		//create the menu bar
		Widgets::Layout* pInternalLayout = new Widgets::Layout();
		pInternalLayout->SetDirection(Widgets::Layout::Direction::Vertical);
		pInternalLayout->SetSizeStyle(Widgets::Widget::SIZE_STYLE::STRETCH);
		pViewportTab->AddWidget(pInternalLayout);

		Widgets::MenuBar* pMenuBar = new Widgets::MenuBar();
		pInternalLayout->AddWidget(pMenuBar);

		//create the file menu
		{
			Widgets::Menu* pFileMenu = pMenuBar->AddMenu("File");

			Widgets::MenuItem* pNewItem = pFileMenu->AddMenuItem("Import...");
			pNewItem->OnClick([this]() { Imports(); });

			Widgets::MenuItem* pSaveItem = pFileMenu->AddMenuItem("Save");
			pSaveItem->OnClick([this]() { OnSaveSelectedMeshClicked(); });
		}

		//create the split
		Widgets::SplitVertical* pSplit = new Widgets::SplitVertical();
		pSplit->SetSizeStyle(Widgets::Widget::HSIZE_STRETCH | Widgets::Widget::VSIZE_STRETCH);
		pSplit->SetLeftPanelWidth(400);
		pInternalLayout->AddWidget(pSplit);

		//create left viewport
		Widgets::Viewport* pViewport = new Widgets::Viewport();
		pViewport->SetSizeStyle(Widgets::Widget::HSIZE_STRETCH | Widgets::Widget::VSIZE_STRETCH);
		pViewport->OnGetRenderTargetTexture([this]() -> Rendering::TextureId { return Rendering::RenderModule::Get().GetRenderTargetTextureId(m_pRenderTarget); });
		pViewport->OnFocusGained([this](const Widgets::FocusEvent&) { m_enableViewportControl = true; });
		pViewport->OnFocusLost([this](const Widgets::FocusEvent&) { m_enableViewportControl = false; });
		pSplit->AddRightPanel(pViewport);

		//split the left panel to, top for the list of meshes, bottom for the logs and materials
		Widgets::SplitHorizontal* pLeftPanelSplit = new Widgets::SplitHorizontal();
		pLeftPanelSplit->SetSizeStyle(Widgets::Widget::HSIZE_STRETCH | Widgets::Widget::VSIZE_STRETCH);
		pSplit->AddLeftPanel(pLeftPanelSplit);

		//create the list of meshes
		Core::Array<const Systems::AssetMetadata*> allMeshes;
		Systems::AssetMgr::Get().GetAssets(MAKESID("Mesh"), allMeshes);

		for(uint32_t ii = 0; ii < allMeshes.GetSize(); ++ii)
		{
			const Systems::AssetMetadata* pMetadata = allMeshes[ii];
			m_allMeshes.push_back(MeshEntry());

			MeshEntry& newEntry = m_allMeshes.back();
			newEntry.m_displayName = pMetadata->GetVirtualName();
			newEntry.m_id = pMetadata->GetAssetId();
			newEntry.m_mesh = nullptr;
		}

		Widgets::Layout* pMeshListLayout = new Widgets::Layout(0, 0, 0, 0);
		pMeshListLayout->SetSizeStyle(Widgets::Widget::HSIZE_STRETCH | Widgets::Widget::VSIZE_STRETCH);
		pMeshListLayout->SetDirection(Widgets::Layout::Direction::Vertical);
		pLeftPanelSplit->AddTopPanel(pMeshListLayout);

		const int LINE_HEIGHT = 20;
		for(int ii = 0; ii < m_allMeshes.size(); ++ii)
		{
			const MeshEntry& entry = m_allMeshes[ii];

			//entry layout
			Widgets::Layout* pEntryLayout = new Widgets::Layout(0, LINE_HEIGHT, 0, 0);
			pEntryLayout->SetSizeStyle(Widgets::Widget::HSIZE_STRETCH | Widgets::Widget::VSIZE_DEFAULT);
			pEntryLayout->SetDirection(Widgets::Layout::Horizontal_Reverse);
			pMeshListLayout->AddWidget(pEntryLayout);

			//import button
			Widgets::Button* pButtonImport = new Widgets::Button(LINE_HEIGHT, LINE_HEIGHT, 0, 0);
			pButtonImport->OnClick([this, ii]() -> bool { return OnMeshImportClicked(ii); });
			pEntryLayout->AddWidget(pButtonImport);

			Widgets::Icon* pImportIcon = new Widgets::Icon(m_importIconTextureId);
			pImportIcon->SetSize(DirectX::XMUINT2(20, 20));
			pButtonImport->AddWidget(pImportIcon);

			//mesh name
			const std::string& meshName = entry.m_displayName;
			Widgets::Button* pButton = new Widgets::Button(0, LINE_HEIGHT, 0, 0);
			pButton->SetSizeStyle(Widgets::Widget::HSIZE_STRETCH | Widgets::Widget::VSIZE_DEFAULT);
			pButton->OnClick([this, ii]() -> bool { OnMeshEntryClicked(ii); return true; });
			pEntryLayout->AddWidget(pButton);
			m_allEntryButton.push_back(pButton);

			const int LABEL_OFFSET_X = 10;
			Widgets::Label* pLabel = new Widgets::Label(LABEL_OFFSET_X, 0, 1, meshName);
			pButton->AddWidget(pLabel);
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
		const std::vector<Systems::Asset*>& allMaterials = Systems::AssetMgr::Get().GetMaterials();
		for(const Systems::Asset* pAssetMaterial : allMaterials)
		{
			//load material
			Rendering::MaterialMgr& materialMgr = Rendering::MaterialMgr::Get();
			Rendering::Material* pMaterial = nullptr;
			Rendering::MaterialId materialId;
			materialMgr.CreateMaterial(&pMaterial, materialId);
			Systems::Loader::Get().LoadMaterial(pAssetMaterial->GetPath(), *pMaterial);

			//create entry
			int materialIndex = static_cast<int>(m_allMaterials.size());
			m_allMaterials.push_back(MaterialEntry());
			MaterialEntry& materialEntry = m_allMaterials.back();
			materialEntry.m_materialId = materialId;
			materialEntry.m_name = pAssetMaterial->GetVirtualName();
			
			//material widget
			Widgets::Button* pButton = new Widgets::Button(0, LINE_HEIGHT, 0, 0);
			pButton->SetSizeStyle(Widgets::Widget::HSIZE_STRETCH | Widgets::Widget::VSIZE_DEFAULT);
			pButton->OnClick([this, materialIndex]() -> bool { return OnMaterialClicked(materialIndex); });
			pMaterialLayout->AddWidget(pButton);

			const int LABEL_OFFSET_X = 10;
			Widgets::Label* pLabel = new Widgets::Label(LABEL_OFFSET_X, 0, 1, materialEntry.m_name);
			pButton->AddWidget(pLabel);
		}

		//by default use the first material
		if(!m_allMaterials.empty())
			m_materialId = m_allMaterials.front().m_materialId;
	}

	void MeshEditor::Update()
	{
		if (m_enableViewportControl)
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
	}

	void MeshEditor::Render()
	{
		m_pRenderTarget->BeginScene();

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
		if (m_selectedMesh != -1)
		{
			const MeshEntry& entry = m_allMeshes[m_selectedMesh];

			DirectX::XMMATRIX mvpMatrix = DirectX::XMMatrixMultiply(world, view);
			mvpMatrix = DirectX::XMMatrixMultiply(mvpMatrix, projection);

			Rendering::RenderModule& renderer = Rendering::RenderModule::Get();

			const Rendering::Material* pMaterial = Rendering::MaterialMgr::Get().GetMaterial(m_materialId);
			renderer.BindMaterial(*pMaterial, mvpMatrix);

			const Rendering::Mesh* pMesh = entry.m_mesh->GetRenderingMesh();// Rendering::MeshMgr::Get().GetMesh(entry.m_meshId);
			renderer.RenderMesh(*pMesh);
		}

		m_pRenderTarget->EndScene();
	}

	void MeshEditor::ShowMesh(int entryIndex)
	{
		m_selectedMesh = entryIndex;
	}

	void MeshEditor::LoadMesh(MeshEntry& entry)
	{
		Systems::ContainerId cid = entry.m_id.GetContainerId();
		Systems::Container* pContainer = Systems::ContainerMgr::Get().LoadContainer(cid);
		Systems::AssetObject* pAsset = pContainer->GetAsset(entry.m_id.GetObjectId());

		entry.m_mesh = static_cast<Systems::MeshAsset*>(pAsset);
	}

	void MeshEditor::OnMeshEntryClicked(int entryIndex)
	{
		//deselect all buttons
		for (Widgets::Button* pButton : m_allEntryButton)
		{
			pButton->Unselect();
		}

		//select new button
		Widgets::Button* pButton = m_allEntryButton[entryIndex];
		pButton->Select();

		MeshEntry& entry = m_allMeshes[entryIndex];
		
		//load the mesh if necessary
		if (!entry.m_mesh)
		{
			LoadMesh(entry);
		}

		ShowMesh(entryIndex);
	}

	bool MeshEditor::OnMeshImportClicked(int entryIndex)
	{
		MeshEntry& entry = m_allMeshes[entryIndex];
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

		return true;
	}

	bool MeshEditor::OnMaterialClicked(int entryIndex)
	{
		const MaterialEntry& materialEntry = m_allMaterials[entryIndex];
		m_materialId = materialEntry.m_materialId;

		return true;
	}

	bool MeshEditor::OnSaveSelectedMeshClicked()
	{
		if (m_selectedMesh == -1)
			return true;

		const MeshEntry& entry = m_allMeshes[m_selectedMesh];
		Systems::ContainerMgr::Get().SaveContainer(entry.m_id.GetContainerId());

		return true;
	}
}
