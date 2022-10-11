/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/MeshEditor/MeshEditor.h"

#include "GameInputs/Inputs/InputMgr.h"

#include "OsWin/Process.h"

#include "Rendering/Material/Material.h"
#include "Rendering/Material/MaterialMgr.h"
#include "Rendering/Mesh/MeshMgr.h"
#include "Rendering/Mesh/Mesh.h"
#include "Rendering/PipelineState/PipelineStateMgr.h"
#include "Rendering/RenderModule.h"
#include "Rendering/RenderTargets/RenderTarget.h"
#include "Rendering/RootSignature/RootSignatureMgr.h"
#include "Rendering/ShaderMgr.h"

#include "Widgets/Button.h"
#include "Widgets/Icon.h"
#include "Widgets/Label.h"
#include "Widgets/Layout.h"
#include "Widgets/SplitHorizontal.h"
#include "Widgets/SplitVertical.h"
#include "Widgets/Tab.h"
#include "Widgets/TabContainer.h"
#include "Widgets/Text.h"
#include "Widgets/Viewport.h"

#include <filesystem>

extern std::string g_shaderRoot;

namespace Editors
{
	MeshEntry::MeshEntry()
		: m_rawFilename()
		, m_binFilename()
		, m_displayName()
		, m_meshId()
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

	void MeshEditor::CreateEditor(Widget* pParent)
	{
		//create the render target
		const int width = 1280;
		const int height = 720;
		m_aspectRatio = width / static_cast<float>(height);
		m_pRenderTarget = RenderModule::Get().CreateRenderTarget(width, height);

		//create the widgets
		Widgets::Tab* pViewportTab = new Widgets::Tab();
		{
			Widgets::TabContainer* pTabContainer = dynamic_cast<Widgets::TabContainer*>(pParent);
			if (pTabContainer)
			{
				pTabContainer->AddTab("Mesh", pViewportTab);
			}
			else
			{
				pParent->AddWidget(pViewportTab);
			}
		}

		//create the split
		Widgets::SplitVertical* pSplit = new Widgets::SplitVertical();
		pSplit->SetSizeStyle(Widget::HSIZE_STRETCH | Widget::VSIZE_STRETCH);
		pSplit->SetLeftPanelWidth(400);
		pViewportTab->AddWidget(pSplit);

		//create left viewport
		Widgets::Viewport* pViewport = new Widgets::Viewport();
		pViewport->SetSizeStyle(Widget::HSIZE_STRETCH | Widget::VSIZE_STRETCH);
		pViewport->OnGetRenderTargetTexture([this]() -> TextureId { return RenderModule::Get().GetRenderTargetTextureId(m_pRenderTarget); });
		pViewport->OnGetFocus([this]() -> bool { m_enableViewportControl = true; return true; });
		pViewport->OnLoseFocus([this]() -> bool { m_enableViewportControl = false; return true; });
		pSplit->AddRightPanel(pViewport);

		//split the left panel to, top for the list of meshes, bottom for the logs and materials
		Widgets::SplitHorizontal* pLeftPanelSplit = new Widgets::SplitHorizontal();
		pLeftPanelSplit->SetSizeStyle(Widget::HSIZE_STRETCH | Widget::VSIZE_STRETCH);
		pSplit->AddLeftPanel(pLeftPanelSplit);

		//create the list of meshes
		const std::string binRoot = "c:\\workspace\\Alpha\\data\\mesh";
		const std::string rawRoot = "c:\\workspace\\Alpha\\raw\\blender";
		for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(rawRoot))
		{
			if (entry.path().extension().string() != ".blend")
				continue;

			m_allMeshes.push_back(MeshEntry());

			MeshEntry& newEntry = m_allMeshes.back();
			newEntry.m_rawFilename = entry.path().string();
			newEntry.m_binFilename = binRoot + "\\" + entry.path().stem().string() + ".json";
			newEntry.m_displayName = entry.path().stem().string();
		}

		Widgets::Layout* pMeshListLayout = new Widgets::Layout(0, 0, 0, 0);
		pMeshListLayout->SetSizeStyle(Widget::HSIZE_STRETCH | Widget::VSIZE_STRETCH);
		pMeshListLayout->SetDirection(Widgets::Layout::Direction::Vertical);
		pLeftPanelSplit->AddTopPanel(pMeshListLayout);

		const int LINE_HEIGHT = 20;
		for(int ii = 0; ii < m_allMeshes.size(); ++ii)
		{
			const MeshEntry& entry = m_allMeshes[ii];

			//entry layout
			Widgets::Layout* pEntryLayout = new Widgets::Layout(0, LINE_HEIGHT, 0, 0);
			pEntryLayout->SetSizeStyle(Widget::HSIZE_STRETCH | Widget::VSIZE_DEFAULT);
			pEntryLayout->SetDirection(Widgets::Layout::Horizontal_Reverse);
			pMeshListLayout->AddWidget(pEntryLayout);

			//import button
			Widgets::Button* pButtonImport = new Widgets::Button(LINE_HEIGHT, LINE_HEIGHT, 0, 0);
			pButtonImport->OnClick([this, ii](int x, int y) -> bool { return OnMeshImportClicked(ii); });
			pEntryLayout->AddWidget(pButtonImport);

			Widgets::Icon* pImportIcon = new Widgets::Icon(DirectX::XMINT2(0, 0), DirectX::XMUINT2(20, 20), "C:\\workspace\\Alpha\\editor\\icons\\import.png");
			pButtonImport->AddWidget(pImportIcon);

			//mesh name
			const std::string& meshName = entry.m_displayName;
			Widgets::Button* pButton = new Widgets::Button(0, LINE_HEIGHT, 0, 0);
			pButton->SetSizeStyle(Widget::HSIZE_STRETCH | Widget::VSIZE_DEFAULT);
			pButton->OnClick([this, ii](int x, int y) -> bool { OnMeshEntryClicked(ii); return true; });
			pEntryLayout->AddWidget(pButton);
			m_allEntryButton.push_back(pButton);

			const int LABEL_OFFSET_X = 10;
			Widgets::Label* pLabel = new Widgets::Label(LABEL_OFFSET_X, 0, 1, meshName);
			pButton->AddWidget(pLabel);
		}

		//bottom split : tab container for materials and logs
		Widgets::TabContainer* pTabContainer = new Widgets::TabContainer();
		pTabContainer->SetSizeStyle(Widget::HSIZE_STRETCH | Widget::VSIZE_STRETCH);
		pLeftPanelSplit->AddBottomPanel(pTabContainer);

		Widgets::Tab* pMaterialTab = new Widgets::Tab();
		pTabContainer->AddTab("Material", pMaterialTab);

		Widgets::Layout* pMaterialLayout = new Widgets::Layout(0, 0, 0, 0);
		pMaterialLayout->SetSizeStyle(Widget::HSIZE_STRETCH | Widget::VSIZE_STRETCH);
		pMaterialLayout->SetDirection(Widgets::Layout::Vertical);
		pMaterialTab->AddWidget(pMaterialLayout);

		Widgets::Tab* pLogTab = new Widgets::Tab();
		pTabContainer->AddTab("Log", pLogTab);

		m_pLogWidget = new Widgets::Text(1, "");
		m_pLogWidget->SetSizeStyle(Widget::HSIZE_STRETCH | Widget::VSIZE_STRETCH);
		pLogTab->AddWidget(m_pLogWidget);

		pTabContainer->SetSelectedTab(0);

		//load the grid orange materials
		{
			Rendering::MaterialMgr& materialMgr = Rendering::MaterialMgr::Get();
			Rendering::Material* pMaterial = nullptr;
			materialMgr.CreateMaterial(&pMaterial, m_materialId);

			std::string root = "C:\\workspace\\Alpha\\data\\shaders\\";
			RootSignatureId rsId = Rendering::RootSignatureMgr::Get().CreateRootSignature(root + "texture.rs.cso");
			ShaderId vsId = g_pShaderMgr->CreateShader(root + "texture.vs.cso");
			ShaderId psId = g_pShaderMgr->CreateShader(root + "texture.ps.cso");

			Rendering::PipelineStateId pid;
			Rendering::PipelineState* pPipelineState = Rendering::PipelineStateMgr::Get().CreatePipelineState(pid);
			pPipelineState->Init_Generic(rsId, vsId, psId);

			std::string textureFilename = "C:\\workspace\\Alpha\\data\\textures\\grid_orange.png";
			TextureId tid;
			Texture* pTexture = RenderModule::Get().GetTextureMgr().CreateResource(tid, textureFilename);
			pTexture->Init(textureFilename);

			pMaterial->Init(rsId, pid);
			pMaterial->SetTexture(tid);

			//material name
			Widgets::Button* pButton = new Widgets::Button(0, LINE_HEIGHT, 0, 0);
			pButton->SetSizeStyle(Widget::HSIZE_STRETCH | Widget::VSIZE_DEFAULT);
			pButton->OnClick([this](int x, int y) -> bool { return OnMaterialClicked(0); });
			pMaterialLayout->AddWidget(pButton);

			const int LABEL_OFFSET_X = 10;
			Widgets::Label* pLabel = new Widgets::Label(LABEL_OFFSET_X, 0, 1, "grid_orange");
			pButton->AddWidget(pLabel);

			//material entry
			m_allMaterials.push_back(MaterialEntry());
			MaterialEntry& materialEntry = m_allMaterials.back();
			materialEntry.m_materialId = m_materialId;
			materialEntry.m_name = "grid_orange";
		}

		//load the vertex color material
		{
			std::string root = "C:\\workspace\\Alpha\\data\\shaders\\";
			RootSignatureId rsId = Rendering::RootSignatureMgr::Get().CreateRootSignature(root + "\\vertex_color.rs.cso");
			ShaderId vsId = g_pShaderMgr->CreateShader(root + "\\vertex_color.vs.cso");
			ShaderId psId = g_pShaderMgr->CreateShader(root + "\\vertex_color.ps.cso");
			
			Rendering::PipelineStateId pid;
			Rendering::PipelineState* pPipelineState = Rendering::PipelineStateMgr::Get().CreatePipelineState(pid);
			pPipelineState->Init_Generic(rsId, vsId, psId);

			Rendering::MaterialMgr& materialMgr = Rendering::MaterialMgr::Get();
			Rendering::Material* pMaterial = nullptr;
			Rendering::MaterialId materialId;
			materialMgr.CreateMaterial(&pMaterial, materialId);
			pMaterial->Init(rsId, pid);

			//material name
			Widgets::Button* pButton = new Widgets::Button(0, LINE_HEIGHT, 0, 0);
			pButton->SetSizeStyle(Widget::HSIZE_STRETCH | Widget::VSIZE_DEFAULT);
			pButton->OnClick([this](int x, int y) -> bool { return OnMaterialClicked(1); });
			pMaterialLayout->AddWidget(pButton);

			const int LABEL_OFFSET_X = 10;
			Widgets::Label* pLabel = new Widgets::Label(LABEL_OFFSET_X, 0, 1, "vertex_color");
			pButton->AddWidget(pLabel);

			//material entry
			m_allMaterials.push_back(MaterialEntry());
			MaterialEntry& materialEntry = m_allMaterials.back();
			materialEntry.m_materialId = materialId;
			materialEntry.m_name = "grid_orange";
		}
	}

	void MeshEditor::Update()
	{
		if (m_enableViewportControl)
		{
			GameInputs::InputMgr& inputs = GameInputs::InputMgr::Get();
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

			RenderModule& renderer = RenderModule::Get();

			const Rendering::Material* pMaterial = Rendering::MaterialMgr::Get().GetMaterial(m_materialId);
			renderer.BindMaterial(*pMaterial, mvpMatrix);

			const Rendering::Mesh* pMesh = Rendering::MeshMgr::Get().GetMesh(entry.m_meshId);
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
		Rendering::Mesh* pCubeMesh = nullptr;
		Rendering::MeshMgr::Get().CreateMesh(&pCubeMesh, entry.m_meshId);
		pCubeMesh->Load(entry.m_binFilename);
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
		if (entry.m_meshId == Rendering::MeshId::INVALID)
		{
			LoadMesh(entry);
		}

		ShowMesh(entryIndex);
	}

	bool MeshEditor::OnMeshImportClicked(int entryIndex)
	{
		MeshEntry& entry = m_allMeshes[entryIndex];

		//re export the json file
		std::string blender = "C:\\Program Files\\Blender Foundation\\Blender 3.0\\blender.exe";
		std::string importCommandline = blender;
		importCommandline += " " + entry.m_rawFilename;
		importCommandline += " --background";
		importCommandline += " --python C:\\workspace\\Alpha\\code\\script\\export_mesh.py";

		OutputDebugString(importCommandline.c_str());

		Process importProcess(importCommandline);
		importProcess.OnStdOut([this](const std::string& msg) -> bool { m_pLogWidget->AppendText(msg.c_str()); return true; });
		importProcess.OnStdErr([this](const std::string& msg) -> bool { m_pLogWidget->AppendText(msg.c_str()); return true; });

		bool started = importProcess.Run();
		if (!started)
		{
			OutputDebugString("Failed to start process\n");
		}

		importProcess.Wait();

		//reload the mesh
		Rendering::MeshMgr& meshMgr = Rendering::MeshMgr::Get();
		Rendering::Mesh* pNewMesh = nullptr;
		Rendering::MeshId newMeshId;

		meshMgr.CreateMesh(&pNewMesh, newMeshId);
		pNewMesh->Load(entry.m_binFilename);

		entry.m_meshId = newMeshId;

		return true;
	}

	bool MeshEditor::OnMaterialClicked(int entryIndex)
	{
		const MaterialEntry& materialEntry = m_allMaterials[entryIndex];
		m_materialId = materialEntry.m_materialId;

		return true;
	}
}
